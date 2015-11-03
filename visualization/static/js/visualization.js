function setupWebsocket(callback) {
    var ws = new WebSocket("ws://localhost:12345/ws?Id=123456789");
    ws.onopen = function() {
        ws.send("Message to send");
    };
    ws.onmessage = function (evt) {
        var received_msg = evt.data;
        callback(JSON.parse(received_msg));
    };
    ws.onclose = function() {
        console.log("WS closed");
    };
}


function setupScene() {
    var scene_root = $("#3dview");

    var scene = new THREE.Scene();
    var camera = new THREE.PerspectiveCamera(
        75,
        scene_root.width() /scene_root.height(),
        0.1,
        1000
    );

    var renderer = new THREE.WebGLRenderer();
    renderer.setSize(scene_root.width(), scene_root.height());
    scene_root.append(renderer.domElement);

    var geometry = new THREE.BoxGeometry( 5, 1, 5 );
    var material = new THREE.MeshLambertMaterial( { color: 0x00ff00 } );
    var cube = new THREE.Mesh( geometry, material );
    scene.add( cube );
    var light = new THREE.AmbientLight( 0x404040 ); // soft white light
    scene.add( light );

    var directionalLight = new THREE.DirectionalLight( 0xffffff, 0.5 );
    directionalLight.position.set( 0, 0, 10 );
    scene.add( directionalLight );

    camera.position.z = 15;

    var render = function () {
	requestAnimationFrame( render );
	renderer.render(scene, camera);
    };

    var coordinateRemapMatrix = new THREE.Matrix4();
    coordinateRemapMatrix.set(
        0,  0,  1,  0,
       -1,  0,  0,  0,
        0, -1,  0,  0,
        0,  0,  0,  1
    );

    function rotateCube(message) {
        var input = JSON.parse(message);
        var q = new THREE.Quaternion();
        var attitude = input["attitude"];
        q.w = attitude[0];
        q.x = attitude[1];
        q.y = attitude[2];
        q.z = attitude[3];
        var m = new THREE.Matrix4();
        m.makeRotationFromQuaternion(q);
//        m.multiply(coordinateRemapMatrix);
        q.setFromRotationMatrix(m);
        cube.quaternion.copy(q);
    }

    render();
}

function setupGraph(root, margins) {
    root = $(root);
    var root_node = root.get(0);
    var margin = margins || {top: 20.5, right: 30, bottom: 30, left: 40.5};
    var width = root.width() - margin.left - margin.right;
    var height = root.height() - margin.top - margin.bottom;

    var xScale = d3.scale.linear()
            .range([0, width]);

    var yScale = d3.scale.linear()
            .range([height, 0]);

    var xAxis = d3.svg.axis()
            .scale(xScale)
            .orient("bottom");

    var yAxis = d3.svg.axis()
            .scale(yScale)
            .orient("left");

    var svg = d3.select(root_node).append("svg")
            .attr("width", width + margin.left + margin.right)
            .attr("height", height + margin.top + margin.bottom)
            .append("g")
            .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    var data = [];
    var xAxisNode = svg.append("g")
            .attr("class", "x axis")
            .attr("transform", "translate(0," + height + ")");

    var yAxisNode = svg.append("g")
            .attr("class", "y axis")
            .call(yAxis)
            .append("text")
            .attr("class", "title")
            .attr("transform", "rotate(-90)")
            .attr("y", 6)
            .attr("dy", ".71em")
            .text("Value");

    var lines = [];
    var line_id = 0;
    var limit = null;

    graph = {
        dataCallback : function (entry) {
            var self = this;
            data.push(entry);
            if(data.length < 2) {
                return;
            }
            if(limit !== null) {
                data = data.slice(-limit);
            }
            var xextents = [];
            var yextents = [];
            _.each(lines, function(line) {
                xextents = xextents.concat(d3.extent(data, line.xf));
                yextents = yextents.concat(d3.extent(data, line.yf));
            });
            xScale.domain(d3.extent(xextents));
            yScale.domain(d3.extent(yextents));
            xAxisNode.call(xAxis);
            yAxisNode.call(yAxis);
            _.each(lines, function(line) {
                svg.select("#" + line.id)   // change the line
                    .attr("d", line.line(data));
            });
        },

        line : function(xf, yf, color) {
            var self = this;
            var id = "line-" + line_id++;

            var line = d3.svg.line()
                    .x(function(d) { return xScale(xf(d)); })
                    .y(function(d) { return yScale(yf(d)); });

            svg.append("path")
                .attr("class", "line")
                .attr("style", "stroke: " + color)
                .attr("id", id)
                .datum(data)
                .attr("d", line);

            lines.push({
                "xf" : xf,
                "yf" : yf,
                "line" : line,
                "id" : id
            });
            return self;
        },
        clear: function() {
            data = [];
        },

        limit: function(new_limit, toggle) {
            if(toggle !== undefined && toggle && new_limit == limit) {
                limit = null;
            } else {
                limit = new_limit;
            }
        }
    };
    _.bindAll(graph, "dataCallback", "line", "clear", "limit");
    return graph;
}
