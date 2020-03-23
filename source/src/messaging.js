// This file handles messages from the client program to the worker
// and sending back responses

self.sendResult = (id, result) => {
  postMessage({ id, result });
};


self.sendError = (id, error) => {
  postMessage({ id, error });
};

self.onmessage_orig = (e) => {
  const msg = e.data;
  const {id, method, args} = msg;

  let result = null;
  let valid = false;

  if (typeof(Module[method]) === 'function') {
    result = Module[method](id, ...args);
    valid = true;
  }

  if (!valid) {
    sendResult(id, result);
  }
  else {
    sendError(id, `Method doesn't exist: ${method}`);
  }
};


self.onmessage = (e) => {
  const msg = e.data;
//  const { id, method, args } = msg; // optimization removes this for some reason

  let valid = false;

  if (typeof(Module[msg.method]) === 'function') {
    Module[msg.method](msg.id, ...msg.args);
    valid = true;
  }

  if (!valid) {
    sendError(msg.id, `Method doesn't exist: ${msg.method}`);
  }
};

postMessage('initialized');
