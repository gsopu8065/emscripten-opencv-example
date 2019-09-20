var Module = {
  onRuntimeInitialized: () => init(Module)
};

function init(Module) {

    var canvas = document.createElement('canvas'), ctx = canvas.getContext('2d');
    var image = document.getElementById('image1');

    canvas.width = image.width;
    canvas.height = image.height;

    ctx.drawImage(image, 0, 0);

    var imageData = ctx.getImageData(0, 0, image.width, image.height);
    var uintArray = imageData.data;

    const uint8_t_ptr = window.Module._malloc(uintArray.length);
    window.Module.HEAPU8.set(uintArray, uint8_t_ptr);

    var v = window.Module.findRectangle(uint8_t_ptr, image.width, image.height);

    var points_array = [];
    for(var i=0;i<4;i++){
        points_array.push(v.get(i).get(0));
        points_array.push(v.get(i).get(1));
    }
    console.log(points_array);

    var birdEyeView = window.Module.birdEyeView(uint8_t_ptr, image.width, image.height, points_array[0], points_array[1], points_array[2], points_array[3], points_array[4], points_array[5], points_array[6], points_array[7]);

    window.Module._free(uint8_t_ptr);

    let AB = distanceBewteenPoints(points_array[0], points_array[1], points_array[2], points_array[3]);
    let CD = distanceBewteenPoints(points_array[4], points_array[5], points_array[6], points_array[7]);
    let BC = distanceBewteenPoints(points_array[2], points_array[3], points_array[4], points_array[5]);
    let AD = distanceBewteenPoints(points_array[0], points_array[1], points_array[6], points_array[7]);

    let maxWidth = AB>CD?AB:CD;
    let maxHeight = BC>AD?BC:AD;

    addImageToDOM(birdEyeView, parseInt(maxWidth, 10), parseInt(maxHeight, 10));

    var x1 = v.get(0).get(0);
    var y1 = v.get(0).get(1);
    var x2,y2;
    for(var i=1;i<=4;i++){


        if(i===4){
            x2 = v.get(0).get(0);
            y2 = v.get(0).get(1);
        }
        else{
            x2 = v.get(i).get(0);
            y2 = v.get(i).get(1);
        }

        drawLine(ctx, x1,y1,x2,y2);
        x1 = x2;
        y1 = y2;
    }
    image.src = canvas.toDataURL("image/jpg");
    image.width = "700";
    image.style = "display:block";
}

function addImageToDOM(data, width, height) {
    //var base64 = btoa(unescape(encodeURIComponent(data))); //btoa(String.fromCharCode.apply(null, data));

    /*console.log(data);
    for(var i=0;i<data.length;i++){
        if((i+1)%4 === 0){
            data[i] = parseInt(255);
        }
        else{
            data[i] = Math.abs(data[i]);
        }
    }
    console.log(data);
*/
    var base64 = numbersToImage(new Uint8ClampedArray(data), width, height);
    console.log(base64);

    var image = document.getElementById('image2');
    image.src = base64;
    image.width = "500";
    //image.style = "display:block";
}

function distanceBewteenPoints(x1, y1, x2, y2) {
    var a = x1 - x2;
    var b = y1 - y2;
    return Math.sqrt( a*a + b*b );
}

function numbersToImage(imageData, width, height) {

    console.log(imageData);

    var cvs = document.createElement('canvas');
    cvs.width = width;
    cvs.height = height;
    let ctx = cvs.getContext('2d');

    var idata = ctx.createImageData(width, height);
    idata.data.set(imageData);

    ctx.putImageData(idata, 0, 0);

    return cvs.toDataURL('image/jpeg', 1);
}



function drawLine(context, x1,y1,x2,y2) {

    context.beginPath();
    context.moveTo(x1,y1);
    context.lineTo(x2,y2);
    context.lineWidth = 10;
    context.strokeStyle = '#00FF00';
    context.stroke();
}
