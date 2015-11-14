function setupScene() {
    var scene_root = $("#view");

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

    var material = new THREE.MeshLambertMaterial( { color: 0x00ff00 } );
    var light = new THREE.AmbientLight( 0x404040 ); // soft white light
    var directionalLight = new THREE.DirectionalLight( 0xffffff, 0.5 );
    directionalLight.position.set( 0, 50, 100 );
    scene.add( directionalLight );

    camera.position.z = 15;
    camera.position.y = 5;

    var render = function () {
	requestAnimationFrame( render );
	renderer.render(scene, camera);
    };

    var m1 = new THREE.Matrix4();
    var mr1 = new THREE.Matrix4();
    mr1.makeRotationZ(Math.PI / 2);
    var mr2 = new THREE.Matrix4();
    mr2.makeRotationX(Math.PI / 2);

    m1.multiplyMatrices(mr1, mr2);

    var m2 = new THREE.Matrix4();
    m2.getInverse(m1);

    var the_pi = null;
    var loader = new THREE.OBJLoader();
    // load a resource
    loader.load(
	// resource URL
	'/static/rpi.obj',
	// Function when resource is loaded
	function ( object ) {
            the_pi = object;
            object.scale.multiplyScalar(.1);
            scene.add(object);
	}
    );

    function rotateCube(input) {
        if(the_pi === null) {
            return;
        }
        var q = new THREE.Quaternion();
        var attitude = input["attitude"];
        q.w = attitude[0];
        q.x = attitude[1];
        q.y = attitude[2];
        q.z = attitude[3];
        var m = new THREE.Matrix4();
        var rs = new THREE.Matrix4();
        var r = new THREE.Matrix4();
        rs.makeRotationFromQuaternion(q);
        r.getInverse(rs);
        m.multiply(m2);
        m.multiply(r);
        m.multiply(m1);
        q.setFromRotationMatrix(m);
        the_pi.quaternion.copy(q);
    }

    render();
    return rotateCube;
}
