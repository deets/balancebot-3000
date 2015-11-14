function setupWebsocket(callback) {
    var ws = new WebSocket("ws://localhost:12345/ws?Id=123456789");
    ws.onopen = function() {
        ws.send("Message to send");
    };
    ws.onmessage = function (evt) {
        var received_msg = evt.data;
        var msg = JSON.parse(received_msg);
        msg.timestamp /= 1000000.0;
        callback(msg);
    };
    ws.onclose = function() {
        console.log("WS closed");
    };
}


$(document).ready(function() {
    var graph = setupGraph("#graph", { "limit" : 300 });
    graph.lines.onValue(function(line_def) {
        var e = $("<span class='legend-entry'/>").css("color", line_def.config.color).text(line_def.config.name);
        $("#legend").append(e);
    });

    graph.limited.onValue(function(v) {
        console.log(v);
        var lb = $("#limit_button");
        if(v) {
            lb.addClass("pressed");
        } else {
            lb.removeClass("pressed");
        }
    });
    graph.line(
        function(d) { return d.timestamp;},
        function(d) { return d.gyroAcc[2]; },
        { "color" : "#ff00ff", global_minmax : true, name: "gyro-acc-Z" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.gyroAcc[1]; },
        { "color" : "#000000", global_minmax : true, name: "gyro-acc-Y" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.gyroAcc[0]; },
        { "color" : "steelblue", global_minmax : true, name: "gyro-acc-X" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccY; },
        { "color" : "blue", global_minmax : true, name: "atan-Y" }
    );

    function toggle3Dscene() {
        if(sceneCb === null) {
            sceneCb = setupScene();
        } else {
            $("#view").empty();
            sceneCb = null;
        }
    }
    $("#clear_button").click(graph.clear);
    $("#limit_button").click(function() { graph.limit(300, true); });
    $("#scene_button").click(toggle3Dscene);

    var sceneCb = null;

    setupWebsocket(function(data) {
        graph.dataCallback(data);
        if(sceneCb !== null) {
            sceneCb(data);
        }
    });

    toggle3Dscene();
});
