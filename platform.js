var sfml = require('./build/Release/node_sfml.node');
//var sfml = require('node_sfml');
//var WebGL = require('./webgl');

module.exports = {
    sfml : sfml,
    document : function () {
  process.on('SIGINT', function () { process.exit(0); });
  
  var window;
  var events;
  
  var platform = {
    type: "nodeSFML",
    setTitle: function(title) {
      if(window)
        window.SetTitle(title);
    },
    setIcon: function(filename) {
      // TODO
    },
    flip: function() {
      if(window)
        window.Display();
    },
    getElementById: function(name) {
      return null;//this;
    },
    createElement: function(name, width, height) {
      if(name.indexOf('canvas')!=-1) {
        this.createWindow(width || 800, height || 800);
        this.canvas = this;
        //WebGL.canvas = this;
        return this;
      }
      return null;
    },
    createWindow : function(width, height) {
      window = new sfml.RenderWindow(new sfml.VideoMode(width, height),
          "SFML window", sfml.Style.Default, new sfml.ContextSettings(24));

      Object.defineProperty(window, 'events', {
        get : function() {
          if (events)
            return events;
          events = new (require('events').EventEmitter);
          var now = Date.now();
          setInterval(function() {
            var after = Date.now();
            var delta = after - now;
            now = after;
            var data;
            // console.log(require('util').inspect(this))
            while (data = window.PollEvent()) {
              data.preventDefault = function () {};
              data.stopPropagation = function () {};
              console.log(require('util').inspect(data))
              events.emit('event', data);
              events.emit(data.type, data);
            }
            events.emit('tick', delta);
          }, 16);
          return events;
        }
      });
      window.events.on('quit', function () { process.exit(0); });
      window.events.on("keydown", function (evt) {
        if (evt.keyCode === 'C'.charCodeAt(0) && evt.ctrlKey) process.exit(0); // Control+C
        if (evt.keyCode === 27) process.exit(0);  // ESC
      });
      this.width = this.drawingBufferWidth=width; 
      this.height = this.drawingBufferHeight=height;
      return window;
    },
    getContext: function(name) {
      return null;//WebGL;
    },
    on: function (name, callback) {
      if(window)
        window.events.addListener(name, callback);
    },
    addEventListener: function(name,callback) {
      if(window)
        window.events.addListener(name, callback);
    },
    removeEventListener: function(name,callback) {
      if(window)
        window.events.removeListener(name,callback);
    },
    requestAnimationFrame: function (callback, delay) {
      if(!window) return;
      
      window.Display();
      var timer= (delay==0) ? process.nextTick : setTimeout;
      var d= (delay==undefined || delay<0) ? 16 : delay;
      timer(function(){
          callback(new Date().getTime());
        }, d);
    }
  };


  Object.defineProperty(platform, 'onkeydown', {
    set: function(cb) {
      this.on('keydown',cb);
    }
  });
  
  Object.defineProperty(platform, 'onkeyup', {
    set: function(cb) {
      this.on('keyup',cb);
    }
  });
  

  return platform;
}
}