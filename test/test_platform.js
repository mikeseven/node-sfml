var platform = require(__dirname + '/../platform.js');
var sfml = platform.sfml;
var document = platform.document();
var util=require('util');
var requestAnimationFrame = document.requestAnimationFrame;
var log=console.log;

var texture=new sfml.Texture();
if (!texture.LoadFromFile(__dirname+"/lena.jpg"))
    process.exit(-1);
console.log("Image "+texture.GetWidth()+"x"+texture.GetHeight());

var sprite=new sfml.Sprite(texture);

// Create the main window
var canvas=document.createWindow(800,800);
sprite.SetScale(800/texture.GetWidth(),800/texture.GetHeight());

canvas.events.on('closed',handleEvent);
canvas.events.on('mousemove',handleEvent);
canvas.events.on('mousebuttondown',handleEvent);
canvas.events.on('keydown',handleEvent);
canvas.events.on('resize',handleEvent);

draw();

function draw() {
  // Clear screen
  canvas.Clear();

  // Draw the sprite
  canvas.Draw(sprite);

  // Draw the string
  //window.Draw(text);

  requestAnimationFrame(draw,10);
}

function handleEvent(event) {
  log('Received event: '+util.inspect(event));
  
  if(event.Type == 'closed') {
    console.log('Window closing');
    canvas.Close();
    process.exit(0);
  }
}

