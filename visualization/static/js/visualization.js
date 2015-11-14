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
        var line_legend_element = $("#legend").find("#" + line_def.id);
        if(line_legend_element.size() == 0) {
            var e = $("<span class='legend-entry'/>").attr("id", line_def.id).css("color", line_def.config.color).text(line_def.config.name).click(
                function() {
                    console.log(line_def.id, line_def.enable(!line_def.enable()));
                }
            );
            $("#legend").append(e);
        } else {
            if(line_def.enable()) {
                line_legend_element.removeClass("disabled");
            } else {
                line_legend_element.addClass("disabled");
            }
        }
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
        { "color" : "#ff00ff", global_minmax : true, name: "gyro-accu-Z" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.gyroAcc[1]; },
        { "color" : "#000000", global_minmax : true, name: "gyro-accu-Y" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.gyroAcc[0]; },
        { "color" : "steelblue", global_minmax : true, name: "gyro-accu-X" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccY; },
        { "color" : "#801515", global_minmax : true, name: "atan-Y" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccX; },
        { "color" : "#d46a6a", global_minmax : true, name: "atan-X" }
    )
    //     .line(
    //     function(d) { return d.timestamp;},
    //     function(d) { return d.debugData.kf.atanAccZ; },
    //     { "color" : "#aa3939", global_minmax : true, name: "atan-Z" }
    // )
    ;

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
