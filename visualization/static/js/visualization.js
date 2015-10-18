function setupWebsocket(callback) {
    var ws = new WebSocket("ws://localhost:12345/ws?Id=123456789");
    ws.onopen = function() {
        ws.send("Message to send");
    };
    ws.onmessage = function (evt) {
        var received_msg = evt.data;
        callback(received_msg);
    };
    ws.onclose = function() {
        console.log("WS closed");
    };
}


$(document).ready(function() {


    console.log("I'm ready!");
    var scene = new THREE.Scene();
    var camera = new THREE.PerspectiveCamera( 75, window.innerWidth/window.innerHeight, 0.1, 1000 );

    var renderer = new THREE.WebGLRenderer();
    renderer.setSize( window.innerWidth, window.innerHeight );
    document.body.appendChild( renderer.domElement );

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
        m.multiply(coordinateRemapMatrix);
        q.setFromRotationMatrix(m);
        cube.quaternion.copy(q);
    }

    setupWebsocket(rotateCube);

    render();
});
