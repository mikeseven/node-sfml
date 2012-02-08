var SFML = module.exports = require('./build/Release/node_sfml.node');

// Easy event emitter based event loop.  Started automatically when the first
// listener is added.
/*var events;
Object.defineProperty(SFML.RenderWindow, 'events', {
  get: function () {
    if (events) return events;
    events = new (require('events').EventEmitter);
    var now = Date.now();
    setInterval(function () {
      var after = Date.now();
      var delta = after - now;
      now = after;
      var data;
      console.log(require('util').inspect(this))
      while (data = this.PollEvent()) {
        events.emit('event', data);
        events.emit(data.Type, data);
      }
      events.emit('tick', delta);
    }, 16);
    return events;
  }
});*/

