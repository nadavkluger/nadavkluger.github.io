using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

/*
    NegaMax (Minimax) algorithm applied on a simple tic tac toe game.
    Author: Nadav Kluger
*/

namespace MiniMaxAlgo
{
    public enum BoardTile
    {
        EMPTY,
        COMPUTER,
        HUMAN
    }

    class MiniMax
    {
        // Logical board - which player is in each square
        private BoardTile[,] BoardState;

        // Scoring arrays
        private int[] ColsEval, RowsEval, DiagsEval;
        private int Sequance;

        // Tie counter
        public int Tie;

        public MiniMax(int size, int Sequance)
        {
            BoardState = new BoardTile[size, size];
            RowsEval = new int[size];
            ColsEval = new int[size];
            DiagsEval = new int[2];
            this.Sequance = Sequance;
        }
        private void UpdateScores(int val, int row, int col)
        {
            RowsEval[row]+=val;
            ColsEval[col]+=val;
            if (row == col) { DiagsEval[0]+=val; }
            if (row + col == BoardState.GetLength(0) - 1) { DiagsEval[1]+=val; }
        }
        public void PutPlayer(BoardTile Player, int row, int col)
        {
            if(Player == BoardTile.COMPUTER) { UpdateScores(1, row, col);}
            else if(Player == BoardTile.HUMAN) { UpdateScores(100, row, col);}
            else { throw new Exception("Internal error, wrong player given to put"); }

            Tie++;
            BoardState[row, col] = Player;
        }

        public void ClearPlayer(BoardTile Player, int row, int col)
        {
            if (Player == BoardTile.COMPUTER){ UpdateScores(-1, row, col); }
            else if (Player == BoardTile.HUMAN) { UpdateScores(-100, row, col); }
            else { throw new Exception("Internal error, wrong player given to clear"); }

            Tie--;
            BoardState[row, col] = BoardTile.EMPTY;
        }

        public Tuple<int, int> MakeBestMove(int Depth)
        {
            int max = int.MinValue + 1;
            int score;
            bool BlockOption;

            Tuple<int, int> BestMove = new Tuple<int, int>(-1, -1);

            for (int row = 0; row < BoardState.GetLength(0); row++)
            {
                for (int col = 0; col < BoardState.GetLength(1); col++)
                {
                    if (BoardState[row, col] == BoardTile.EMPTY)
                    {
                        #region BlockOption
                        PutPlayer(BoardTile.HUMAN, row, col);
                        BlockOption = WinCheck(row, col);
                        ClearPlayer(BoardTile.HUMAN, row, col);
                        #endregion

                        PutPlayer(BoardTile.COMPUTER, row, col);

                        if(WinCheck(row, col)) { return new Tuple<int, int>(row, col); }

                        if(BlockOption) { score = int.MaxValue; }
                        else { score = -NegaMax(false, Depth - 1, -max); }

                        ClearPlayer(BoardTile.COMPUTER, row, col);

                        if (max < score)
                        {
                            max = score;
                            BestMove = new Tuple<int, int>(row, col);
                        }
                    }
                }
            }

            PutPlayer(BoardTile.COMPUTER, BestMove.Item1, BestMove.Item2);
            return BestMove;
        }

        private int NegaMax(bool CompTurn, int Depth, int PrevScore)
        {
            if (Depth == 0 || Tie == BoardState.Length) { return EvalFunc(CompTurn); }

            int row, col;
            int extreme = int.MinValue + 1;
            BoardTile Player = CompTurn ? BoardTile.COMPUTER : BoardTile.HUMAN;
            int score = 0;

            for (row = 0; row < BoardState.GetLength(0); row++)
            {
                for (col = 0; col < BoardState.GetLength(1); col++)
                {
                    if (BoardState[row, col] == BoardTile.EMPTY)
                    {
                        PutPlayer(Player, row, col);

                        if (WinCheck(row, col)) { score = int.MaxValue - 1; }
                        else { score = -NegaMax(!CompTurn, Depth - 1, -extreme); }

                        ClearPlayer(Player, row, col);

                        if (score >= PrevScore) { return score; }
                        if (score > extreme) { extreme = score; }
                    }
                }
            }

            return extreme;
        }

        private int EvalFunc(bool CompTurn)
        {
            byte i;
            int CompScore = 0;
            int PlyrScore = 0;

            for (i = 0; i < RowsEval.Length; i++)
            {
                if (RowsEval[i] < 100) { CompScore += RowsEval[i]; }
                else if (RowsEval[i] % 100 == 0) { PlyrScore += RowsEval[i] / 100; }
            }

            for (i = 0; i < ColsEval.Length; i++)
            {
                if (ColsEval[i] < 100) { CompScore += ColsEval[i]; }
                else if (ColsEval[i] % 100 == 0) { PlyrScore += ColsEval[i] / 100; }
            }

            for (i = 0; i < DiagsEval.Length; i++)
            {
                if (DiagsEval[i] < 100) { CompScore += DiagsEval[i]; }
                else if (DiagsEval[i] % 100 == 0) { PlyrScore += DiagsEval[i] / 100; }
            }

            return (CompTurn ? (CompScore - PlyrScore) : (PlyrScore - CompScore));
        }

        public bool WinCheck(int Row, int Col)
        {
            if (RowsEval[Row] == Sequance) { return true; }
            if (RowsEval[Row] == Sequance * 100) { return true; }
            if (ColsEval[Col] == Sequance) { return true; }
            if (ColsEval[Col] == Sequance * 100) { return true; }
            for (int i = 0; i < DiagsEval.Length; i++)
            {
                if (DiagsEval[i] == Sequance) { return true; }
                if (DiagsEval[i] == Sequance * 100) { return true; }
            }

            return false;
        }
    }
}
