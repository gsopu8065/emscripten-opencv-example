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

    window.Module._free(uint8_t_ptr);

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

function drawLine(context, x1,y1,x2,y2) {

    context.beginPath();
    context.moveTo(x1,y1);
    context.lineTo(x2,y2);
    context.lineWidth = 10;
    context.strokeStyle = '#00FF00';
    context.stroke();
}
