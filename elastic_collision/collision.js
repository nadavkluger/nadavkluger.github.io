var canvasObj, contextObj;
var WIDTH = 750, HEIGHT = 500;
var balls, interval;
var init = function(objs) {
			document.getElementById('label').innerHTML=objs;
			document.getElementById("canvasFrame").setAttribute("width", WIDTH);
			document.getElementById("canvasFrame").setAttribute("HEIGHT", HEIGHT);
			balls = new Array(objs);
			for (var i = 0; i < objs; i++) {
	        balls[i] = new Ball();
	    }
	    canvasObj = document.getElementById("canvasFrame");
	    contextObj = canvasObj.getContext("2d");
			interval = setInterval(draw, 10, objs);

}
var update = function(objs){
	document.getElementById('label').innerHTML=objs;
	clearInterval(interval);
			if(balls.length>objs){
				balls.splice(0,(balls.length-objs));
			}
			else if(balls.length<objs){
				for (var i = 0; i < (objs-balls.length); i++) {
				balls.push(new Ball());
				}
			}
	interval = setInterval(draw, 10, objs);
}
var Ball = function(){
      this.radius= Math.floor(Math.random() * 20)+5;
      this.vel =new Vector(Math.floor(Math.random() * 1.75) + 1, Math.floor(Math.random() * 1.75) + 1);
      this.pos = new Vector(Math.floor(Math.random() * (WIDTH - 2 * this.radius)) +this.radius,
			 										Math.floor(Math.random() * (HEIGHT - 2 * this.radius)) + this.radius);
      this.updatePosition = function(){
        if (this.vel.x > 0 && this.pos.x + this.radius >= WIDTH) {
            this.vel.x = -this.vel.x;
        }
        if (this.vel.x < 0 && this.pos.x - this.radius <= 0) {
            this.vel.x = -this.vel.x;
        }
        if (this.vel.y > 0 && this.pos.y +this.radius >= HEIGHT) {
            this.vel.y = -this.vel.y;
        }
        if (this.vel.y < 0 && this.pos.y - this.radius <= 0) {
            this.vel.y = -this.vel.y;
        }
        this.pos.x += this.vel.x;
        this.pos.y += this.vel.y;
      }
}
var clear = function() {
    contextObj.rect(0, 0, WIDTH, HEIGHT);
    contextObj.fill();
}
var draw = function(objs) {
    contextObj.fillStyle = "#FAF7F8";
    clear();
    contextObj.fillStyle = "#444444";

    //UPDATE POSITION
    for (var i = 0; i < balls.length; i++) {
        balls[i].updatePosition();
    }
    //CALCULATE COLLISION
    for (var i = 0; i < balls.length; i++) {
		contextObj.beginPath();
		contextObj.arc(balls[i].pos.x, balls[i].pos.y, balls[i].radius, 0, 2 * Math.PI, false);
		contextObj.fill();
        for (var j = 0; j < balls.length; j++) {
            if (i == j) {
                continue;
            }
		if(detectCollision(i,j)){ fixCollision(i, j) }
        }
    }
}
var detectCollision= function(i,j) {
	var distance = Math.sqrt(((balls[i].pos.x - balls[j].pos.x) * (balls[i].pos.x - balls[j].pos.x)) +
	 												((balls[i].pos.y - balls[j].pos.y) * (balls[i].pos.y - balls[j].pos.y)));
	if (distance <= balls[i].radius + balls[j].radius) {
		return true;
	}
	return false;
}
var fixCollision= function(i, j) {
        //Radius is propertianol to mass
        //Elastic collision equation for 2 dimensions
        var m1 = balls[i].radius;
        var m2 = balls[j].radius;
        var v1 = balls[i].vel;
        var v2 = balls[j].vel;
        var x1 = balls[i].pos;
        var x2 = balls[j].pos;
        var dv1 = v1.subtract(v2);
        var dv2 = v2.subtract(v1);
        var dx1 = x1.subtract(x2);
        var dx2 = x2.subtract(x1);

        balls[i].vel = v1.subtract(dx1.multiply(((2 * m2) / (m1 + m2)) * dv1.dot(dx1) / (dx1.length() * dx1.length())));
        balls[j].vel = v2.subtract(dx2.multiply(((2 * m1) / (m1 + m2)) * dv2.dot(dx2) / (dx2.length() * dx2.length())));


        balls[i].pos = balls[i].pos.add(balls[i].vel);
        balls[j].pos = balls[j].pos.add(balls[j].vel);
}
