module.exports = function(RED) {
    "use strict";
    var I2C = require("i2c-bus");
    // The RTD Node
    function RTDNode(n) {
        RED.nodes.createNode(this, n);
        this.stack = parseInt(n.stack);
        this.channel = parseInt(n.channel);
        this.payload = n.payload;
        this.payloadType = n.payloadType;
        var node = this;
        var buffer = Buffer.alloc(4);
        
        node.port = I2C.openSync( 1 );
        node.on("input", function(msg) {
            var myPayload;
            var stack = node.stack; 
            if (isNaN(stack)) stack = msg.stack;
            var channel = node.channel;
            if (isNaN(channel)) channel = msg.channel;
            stack = parseInt(stack);
            channel = parseInt(channel);
            //var buffcount = parseInt(node.count);
            if (isNaN(stack+1)) {
                this.status({fill:"red",shape:"ring",text:"Stack level ("+stack+") value is missing or incorrect"});
                return;
            } else if (isNaN(channel) ) {
                this.status({fill:"red",shape:"ring",text:"Sensor number  ("+channel+") value is missing or incorrect"});
                return;
            } else {
                this.status({});
            }
            try {
                var hwAdd = 0x40;
                if(stack < 0){
                    stack = 0;
                }
                if(stack > 7){
                  stack = 7;
                }
                hwAdd += stack;
                
                if(channel < 1){
                  channel = 1;
                }
                if(channel > 8){
                  channel = 8;
                }
                
                if (this.payloadType == null) {
                    myPayload = this.payload;
                } else if (this.payloadType == 'none') {
                    myPayload = null;
                } else {
                    myPayload = RED.util.evaluateNodeProperty(this.payload, this.payloadType, this,msg);
                }
                node.port.readI2cBlock(hwAdd, (channel - 1)*4, 4, buffer,  function(err, size, res) {
                    if (err) { 
                        node.error(err, msg);
                    } 
                    else{
                        msg.payload = res.readFloatLE(0).toFixed(4);                       
                        node.send(msg);
                    }
                    });     
                    
            } catch(err) {
                this.error(err,msg);
            }
            
        });

        node.on("close", function() {
            node.port.closeSync();
        });
    }
    RED.nodes.registerType("rtd", RTDNode);

}
