
COLORS = [
    "#FDB8A9",
    "#D37C6A",
    "#AA4C39",
    "#812815",
    "#570F00",
    "#877BAE",
    "#605192",
    "#403075",
    "#271758",
    "#13073C",
    "#FDF5A9",
    "#D3CA6A",
    "#AAA039",
    "#817715",
    "#574F00"
]

color_index = 0;

function pick_color() {
    var c =COLORS[color_index];
    color_index = (color_index + COLORS.length / 3 + 1) % COLORS.length;
    return c;
}

function deg2rad(d) {
    return d / 180.0 * Math.PI;
}

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
        function(d) { return deg2rad(d.gyroAcc[2]); },
        { "color" : pick_color(), global_minmax : true, name: "gyro-accu-Z" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return deg2rad(d.gyroAcc[1]); },
        { "color" : pick_color(), global_minmax : true, name: "gyro-accu-Y" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return deg2rad(d.gyroAcc[0]); },
        { "color" : pick_color(), global_minmax : true, name: "gyro-accu-X" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccY; },
        { "color" : pick_color(), global_minmax : true, name: "atan-Y" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccX; },
        { "color" : pick_color(), global_minmax : true, name: "atan-X" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccXRaw; },
        { "color" : pick_color(), global_minmax : true, name: "atan-X-raw" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.atanAccYRaw; },
        { "color" : pick_color(), global_minmax : true, name: "atan-Y-raw" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.acc[0]; },
        { "color" : pick_color(), global_minmax : true, name: "accX" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.acc[1]; },
        { "color" : pick_color(), global_minmax : true, name: "accY" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.acc[2]; },
        { "color" : pick_color(), global_minmax : true, name: "accZ" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return Math.sqrt(Math.pow(d.acc[0], 2) + Math.pow(d.acc[1], 2) + Math.pow(d.acc[2], 2)); },
        { "color" : pick_color(), global_minmax : true, name: "acc length" }
    ).line(
        function(d) { return d.timestamp;},
        function(d) { return d.debugData.kf.accOutsideThreshold; },
        { "color" : pick_color(), global_minmax : true, name: "acc outside threshold" }
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
        console.log(data);
        if(sceneCb !== null) {
            sceneCb(data);
        }
    });

    toggle3Dscene();
});
