$(function () {
    function resizeCanvas() {
        var height = window.innerHeight;
        var is_safari = navigator.userAgent.indexOf("Safari") > -1;

        if ((navigator.userAgent.match(/iPhone/i)) ||
            (navigator.userAgent.match(/iPod/i))) {
            if (is_safari) {
                height += 80;
            }
        }
        $('#myCanvas').attr("width", window.innerWidth).attr("height", height);
    }
    window.addEventListener('resize', resizeCanvas);
    resizeCanvas();

    // 禁止移动端弹性webview
    document.ontouchmove = function (event) {
        event.preventDefault();
    }
	
})

var regex = /\S*(qq|q-q|qun|wg|w-g|qvn|fz|f-z)\S*/i;
var FPS = 0;
setInterval(function(){
		console.log("FPS:"+FPS);
		FPS=0;
	},1000);
$(function () {
    // setup context and database connection
    var canvas = $('#myCanvas')[0];
    var ctx = canvas.getContext('2d');
    trackTransforms(ctx);
    var factor = 1;
    var scaledFactor = 1;
    var socket = io();
    var locations = {};
    var mapImage = new Image;
    var mapType = parseInt(getParameterByName('map') || 1);
    if(mapType == 1) {
        mapImage.src = "map.jpg"
    }else if (mapType ==2) {
        mapImage.src = "mapMiramar.jpg"
    }
    var trackPlayerIndex = parseInt(getParameterByName('id') || 0);
    var trackPlayerName = getParameterByName('name');
    var separated = {};
    var viewPointOffset = {
        x: window.innerWidth / 2,
        y: window.innerHeight / 2
    };



    window.addEventListener('resize', function () {
        viewPointOffset = {
            x: window.innerWidth / 2,
            y: window.innerHeight / 2
        };
    });

    var drawing = false;
    socket.on('update', function (snapshot) {
        locations = snapshot;
		FPS++;
        if (!drawing) {
            redraw();
        }
    });
    var key = [ 1,3,4,6,7 ];//加密字符
    function decryption(string)
    {
        var resutl = "";
        for(var i = 0; i < string.length; i ++) {
            if (string.charAt(i) == '{' || string.charAt(i) == ':' || string.charAt(i) == '}') {
                resutl += string.charAt(i);
            }
            result += string.charAt(i) ^ key[i % 5];
        }
        return resutl;
    }

    function redraw() {
        drawing = true;
        // Clear the entire canvas
        var p1 = ctx.transformedPoint(0, 0);
        var p2 = ctx.transformedPoint(canvas.width, canvas.height);
        ctx.clearRect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

        // 视角追踪

        /**if(trackPlayerName){
            var players = locations.p;
            for (var i = players.length - 1; i >= 0; i--) {
                if(players[i].name == trackPlayerName){
                    trackPlayerIndex = i;
                }
            }
        }*/
        if(!locations || !locations.p) {
			
            return;
        }
        var players = locations.p;
        for (var i = players.length - 1; i >= 0; i--) {
            if(players[i].name == trackPlayerName){
                trackPlayerIndex = i;
            }else if(players[i].local == true) {
                trackPlayerIndex = i;
            }

        }

        if (locations.p && locations.p[trackPlayerIndex]) {
            var player = locations.p[trackPlayerIndex];
            var centerX = game2pix(player.x);
            var centerY = game2pix(player.y);

            // 相对于上次偏移了多少？
            ctx.translate(viewPointOffset.x - centerX, viewPointOffset.y - centerY);
            viewPointOffset.x = centerX;
            viewPointOffset.y = centerY;
        }
        // draw map
        ctx.drawImage(mapImage, 0, 0);


        drawPlayers(ctx);

        drawItems(ctx);
        drawVehicles(ctx);

        drawSafeZone(ctx);
        setTimeout(function () {
            drawing = false;
        }, 0);

    }

    function drawPlayers(ctx) {
        if (!locations.p) {
            return;
        }
        var players = locations.p;
        var trackPlayer;
        /*if(trackPlayerName){
            for (var i = players.length - 1; i >= 0; i--) {
                if(players[i].name == trackPlayerName){
                    trackPlayer = players[i];
                }
            }
        }*/
        for (var i = players.length - 1; i >= 0; i--) {
            if(trackPlayerName){
                if(players[i].name == trackPlayerName){
                    trackPlayer = players[i];
                }
            }else if(players[i].local) {
                trackPlayer = players[i];
            }
        }
        if(!trackPlayer) {
            trackPlayer = players[trackPlayerIndex];
        }





        for (var i = players.length - 1; i >= 0; i--) {
            var player = players[i];
            var color = "";
            if (player == trackPlayer) {
                color = '#00BB00';
            } else if ((!trackPlayer.t?0:trackPlayer.t) == player.t) {
                color = '#0033BB';
            } else {
                if(regex.test(player.name)){
                    color = '#FF00CC';
                }else{
                    color = '#ff0000';
                }

            }
            if (player.hp == 0) {
                color = '#000000';
                drawDot(ctx, player.x, player.y, color);
            } else {
                drawLineWithAngle(ctx, player.x, player.y, 15 / scaledFactor, 6 / scaledFactor, player.r, color);
                drawDotWithHp(ctx, player.x, player.y, player.hp, color);
            }
            drawText(ctx, player.x, player.y, i, 'white');
            //draw distance
            if(trackPlayer && player != trackPlayer) {
                var _distance = Math.round(Math.sqrt(Math.pow((trackPlayer.x - player.x),2) + Math.pow((trackPlayer.y - player.y),2) + Math.pow((trackPlayer.z - player.z),2))/100.0);
                var flag = "";
                if((trackPlayer.z - player.z)/100.0 > 3 ){
                    flag = "-";
                }else if((trackPlayer.z - player.z)/100.0 < -3 ) {
                    flag = "+";
                }
                var distance = _distance + flag;
                if(_distance > 600) {drawDistanceText(ctx, player.x, player.y, distance, "#FF6600");}
                else if (_distance > 400){drawDistanceText(ctx, player.x, player.y, distance, "#996600");}
                else if (_distance > 200){drawDistanceText(ctx, player.x, player.y, distance, "#FFCC33");}
                else if (_distance >= 10){drawDistanceText(ctx, player.x , player.y, distance, "#00CC00");}
                else {drawDistanceText(ctx, player.x , player.y, flag, "#00CC00");}

                //TODO  draw name
                drawPlayerName(ctx, player.x , player.y, player.name, "#FFFFFF");
            }
        }
    }

    var teamTempVar = -1;
    function drawItems(ctx) {
        if (!locations.i) {
            return;
        }
        var itemArray = itemMap[itemWanted];
        var items = locations.i;
        for (var i = items.length - 1; i >= 0; i--) {
            var item = items[i];
            if(itemArray.contains(item.n)){
                drawText(ctx, item.x, item.y, item.n, 'red');
            }
            // drawDot(ctx, item.x, item.y, 'red', 1);
        }
    }

    function drawVehicles(ctx) {
        if (!locations.v) {
            return;
        }
        var vehicles = locations.v;
        for (var i = vehicles.length - 1; i >= 0; i--) {
            var vehicle = vehicles[i];
            drawText(ctx, vehicle.x, vehicle.y, vehicle.v, 'orange');
        }
    }

    function drawSafeZone(ctx) {
        var zone = locations.zone;
        if (!zone) {
            return;
        }
        if(zone.white) {
            drawCircle(ctx, zone.white.x,zone.white.y,zone.white.r/100,'white')
        }
        if(zone.blue) {
            drawCircle(ctx, zone.blue.x,zone.blue.y,zone.blue.r/100,'blue')
        }
    }

    function drawCircle(ctx, x, y,redius, color, width) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        ctx.beginPath();
        ctx.arc(centerX, centerY, redius, 0, 2 * Math.PI, false);
        ctx.lineWidth = width || 5;
        ctx.strokeStyle = color;
        ctx.stroke();//画空心圆
    }

    function drawDot(ctx, x, y, color, width) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        var radius = 7 / scaledFactor;
        ctx.beginPath();
        ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
        ctx.lineWidth = width || 5;
        ctx.fillStyle = color;
        ctx.fill();
    }

    function drawDotWithHp(ctx, x, y, hp, color) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);

        // 底色
        drawDot(ctx, x, y, color);


        var radius = 7 / scaledFactor;
        var startAngle = 1.5 * Math.PI;
        var endAngle = (((100 - hp) / 100) * 2 * Math.PI) + 1.5 * Math.PI;

        // 扇形
        ctx.fillStyle = 'gray';
        ctx.beginPath();
        ctx.moveTo(centerX, centerY);
        ctx.arc(centerX, centerY, radius, startAngle, endAngle, false);
        ctx.closePath();
        ctx.fill();
        startAngle = endAngle;

    }

    function drawText(ctx, x, y, content, color) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        ctx.font = '' + 11 / scaledFactor + 'pt Calibri';
        ctx.fillStyle = color;
        ctx.textAlign = 'center';
        ctx.fillText(content, centerX, centerY + (3 / scaledFactor));
    }
    function drawDistanceText(ctx, x, y, content, color) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        ctx.font = '' + 10 / scaledFactor + 'pt Calibri';
        ctx.fillStyle = color;
        ctx.textAlign = 'center';
        ctx.fillText(content, centerX , centerY + (15 / scaledFactor));
    }
    function drawPlayerName(ctx, x, y, content, color) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        ctx.font = '' + 10 / scaledFactor + 'pt Calibri';
        ctx.fillStyle = color;
        ctx.textAlign = 'center';
        ctx.fillText(content, centerX , centerY - (13 / scaledFactor));
    }


    // from https://github.com/jerrytang67/helloworld
    function drawLineWithAngle(ctx, x, y, length, width, angle, color) {
        var centerX = game2pix(x);
        var centerY = game2pix(y);
        var anX = 5 * Math.cos(Math.PI * angle / 180.0);
        var anY = 5 * Math.sin(Math.PI * angle / 180.0);

        var x1 = centerX + anX;
        var y1 = centerY + anY;

        var circle1 = {
            x: centerX,
            y: centerY,
            r: 5
        };
        var circle2 = {
            x: x1,
            y: y1,
            r: 0
        };

        var arrow = {
            h: width,
            w: length
        };

        drawArrow(ctx, arrow, circle1, circle2, color);
    }

    //draw arrow -- uuaing
    function drawArrow(canvasContext, arrow, ptArrow, endPt, color) {
        var angleInDegrees = getAngleBetweenPoints(ptArrow, endPt);
        var endPt = getPointOnCircle(endPt.r, ptArrow, endPt);
        // first save the untranslated/unrotated context
        canvasContext.save();

        // move the rotation point to the center of the rect    
        canvasContext.translate(endPt.x, endPt.y);
        // rotate the rect
        canvasContext.rotate(angleInDegrees * Math.PI / 180);
        canvasContext.beginPath();
        canvasContext.moveTo(0, 0);

        canvasContext.lineTo(0, -arrow.h);
        canvasContext.lineTo(arrow.w, 0);
        canvasContext.lineTo(0, +arrow.h);
        canvasContext.closePath();
        canvasContext.fillStyle = color;
        canvasContext.lineWidth = 0;
        //canvasContext.stroke();
        canvasContext.fill();

        // restore the context to its untranslated/unrotated state
        canvasContext.restore();
    }

    function getPointOnCircle(radius, originPt, endPt) {
        var angleInDegrees = getAngleBetweenPoints(originPt, endPt);
        // Convert from degrees to radians via multiplication by PI/180        
        var x = radius * Math.cos(angleInDegrees * Math.PI / 180) + originPt.x;
        var y = radius * Math.sin(angleInDegrees * Math.PI / 180) + originPt.y;
        return {
            x: x,
            y: y
        };
    }

    function getAngleBetweenPoints(originPt, endPt) {
        var interPt = {
            x: endPt.x - originPt.x,
            y: endPt.y - originPt.y
        };
        return Math.atan2(interPt.y, interPt.x) * 180 / Math.PI;
    }

    var hammertime = new Hammer.Manager(canvas);
    hammertime.add(new Hammer.Pan({
        threshold: 0
    }));
    hammertime.add(new Hammer.Pinch({
        threshold: 0
    }))


    // 拖动
    var lastDelta = {
        x: 0,
        y: 0
    }
    hammertime.on('panmove', function (ev) {
        ctx.translate(ev.deltaX - lastDelta.x, ev.deltaY - lastDelta.y);
        lastDelta.x = ev.deltaX;
        lastDelta.y = ev.deltaY;
        redraw();
    });
    hammertime.on('panend', function (ev) {
        lastDelta = {
            x: 0,
            y: 0
        }
    });

    // 缩放
    // 手势
    var lastScale = 0;
    hammertime.on('pinchmove', function (ev) {
        zoomByScale(lastScale + (lastScale - ev.scale));
        lastScale = ev.scale;
    });

    // 滚轮
    function mouseScroll(evt) {
        var delta = evt.wheelDelta ? evt.wheelDelta / 40 : evt.detail ? -evt.detail : 0;
        if (delta) {
            zoomByScale(Math.pow(scaleFactor, delta));
        }
        return evt.preventDefault() && false;
    }
    canvas.addEventListener('DOMMouseScroll', mouseScroll, false);
    canvas.addEventListener('mousewheel', mouseScroll, false);

    var scaleFactor = 1.1;

    function zoomByScale(scale) {
        scaledFactor *= scale;
        var pt = ctx.transformedPoint(window.innerWidth / 2, window.innerHeight / 2);
        ctx.translate(pt.x, pt.y);
        ctx.scale(scale, scale);
        ctx.translate(-pt.x, -pt.y);
        redraw();
    }

    function getParameterByName(name, url) {
        if (!url) url = window.location.href;
        name = name.replace(/[\[\]]/g, "\\$&");
        var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
            results = regex.exec(url);
        if (!results) return null;
        if (!results[2]) return '';
        return decodeURIComponent(results[2].replace(/\+/g, " "));
    }

    // translates game coords to overlay coords
    function game2pix(p) {
        return p * (8130 / 813000)
    }

    // Adds ctx.getTransform() - returns an SVGMatrix
    // Adds ctx.transformedPoint(x,y) - returns an SVGPoint
    function trackTransforms(ctx) {
        var svg = document.createElementNS("http://www.w3.org/2000/svg", 'svg');
        var xform = svg.createSVGMatrix();
        ctx.getTransform = function () {
            return xform;
        };

        var savedTransforms = [];
        var save = ctx.save;
        ctx.save = function () {
            savedTransforms.push(xform.translate(0, 0));
            return save.call(ctx);
        };
        var restore = ctx.restore;
        ctx.restore = function () {
            xform = savedTransforms.pop();
            return restore.call(ctx);
        };

        var scale = ctx.scale;
        ctx.scale = function (sx, sy) {
            xform = xform.scaleNonUniform(sx, sy);
            return scale.call(ctx, sx, sy);
        };
        var rotate = ctx.rotate;
        ctx.rotate = function (radians) {
            xform = xform.rotate(radians * 180 / Math.PI);
            return rotate.call(ctx, radians);
        };
        var translate = ctx.translate;
        ctx.translate = function (dx, dy) {
            xform = xform.translate(dx, dy);
            return translate.call(ctx, dx, dy);
        };
        var transform = ctx.transform;
        ctx.transform = function (a, b, c, d, e, f) {
            var m2 = svg.createSVGMatrix();
            m2.a = a;
            m2.b = b;
            m2.c = c;
            m2.d = d;
            m2.e = e;
            m2.f = f;
            xform = xform.multiply(m2);
            return transform.call(ctx, a, b, c, d, e, f);
        };
        var setTransform = ctx.setTransform;
        ctx.setTransform = function (a, b, c, d, e, f) {
            xform.a = a;
            xform.b = b;
            xform.c = c;
            xform.d = d;
            xform.e = e;
            xform.f = f;
            return setTransform.call(ctx, a, b, c, d, e, f);
        };
        var pt = svg.createSVGPoint();
        ctx.transformedPoint = function (x, y) {
            pt.x = x;
            pt.y = y;
            return pt.matrixTransform(xform.inverse());
        }
    }


});

var itemWanted = 0;
var itemMap = [
    ["h3","v3","b3","m16","m4","scar","mini","sks","98k","sSr","4x","8x","sAr","sreq","ms"],
    ["1897","uzi","ump","tps","s12","cb","s686","m16","m4","scar","mini","sks","98k","ak"],
    ["sSr","sAr","cAr","cSr","fSr","fAr","afg","cb","arp","srp","areq","sreq","are","sre","1x","4x","8x"],
    ["ms","pk","ed","556","762","gas"],
    ["h2","v2","h3","v3"],
	[]
];
function itemChange(value) {
    itemWanted = value;
    console.log(itemWanted);
}
Array.prototype.contains = function ( needle ) {
    for (i in this) {
        if (this[i] == needle) return true;
    }
    return false;
}