var sfml = require(__dirname + '/../index.js');
var log=console.log;

var texture=new sfml.Texture();
if (!texture.LoadFromFile(__dirname+"/lena.jpg"))
    process.exit(-1);
console.log("Image "+texture.GetWidth()+"x"+texture.GetHeight());

var sprite=new sfml.Sprite(texture);
sprite.SetScale(0.5,0.5);

// Create the main window
var window=new sfml.RenderWindow(new sfml.VideoMode(texture.GetWidth()/2,texture.GetHeight()/2), 
                                  "SFML window",sfml.Style.Default, new sfml.ContextSettings(32));
window.SetFramerateLimit(30);

var events;
Object.defineProperty(window, 'events', {
  get: function () {
    if (events) return events;
    events = new (require('events').EventEmitter);
    var now = Date.now();
    setInterval(function () {
      var after = Date.now();
      var delta = after - now;
      now = after;
      var data;
      //console.log(require('util').inspect(this))
      while (window.IsOpen() && (data = window.PollEvent()) ) {
        console.log(require('util').inspect(data))
        events.emit('event', data);
        events.emit(data.Type, data);
      }
      events.emit('tick', delta);
    }, 16);
    return events;
  }
});

var done=false;
window.events.on('CLOSED',handleEvent);

var timer=setTimeout(function() {
  
  // Clear screen
  window.Clear();

  // Draw the sprite
  window.Draw(sprite);

  // Draw the string
  //window.Draw(text);

  // Display
  window.Display();
},16);


function handleEvent(event) {
  log('Received event: '+event);
  
  // event processing
  if(event.Type=='CLOSED') {
    done=true;
    console.log('Window closing');
    clearTimeout(timer);
    window.Close();
    process.exit(0);
  }
  return false;
}