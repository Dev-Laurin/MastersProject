//groundPlaneDisplay.js
//Laurin Fisher
//9-6-18
//To draw the ground planes produced by RANSAC with Kinect v2 data. 

//GLOBALS 
//Set the scene size. 
const WIDTH = 800; 
const HEIGHT = 300; 

//Set some camera attributes. 
const VIEW_ANGLE = 45; 
const ASPECT = WIDTH / HEIGHT; 
const NEAR = 0.1; 
const FAR = 10000; 

//Get the DOM element to attach to 
const container = document.querySelector('#container'); 

//Create a WebGL renderer, camera, and a scene 
const renderer = new THREE.WebGLRenderer(); 
const camera = 
	new THREE.PerspectiveCamera(
		VIEW_ANGLE, 
		ASPECT, 
		NEAR, 
		FAR
	); 

const scene = new THREE.Scene(); 

//Plane 
//(width, height, width segments, height segments)
var geometry = new THREE.PlaneGeometry(10, 10, 32); 
var material = new THREE.MeshBasicMaterial( {
	color: 0xffff00, side: THREE.DoubleSide
}); 
var plane = new THREE.Plane(new THREE.Vector3(1, 1, 0.2), 3);
var helper = new THREE.PlaneHelper(plane, 100, 0xffff00); 
//scene.add(helper);   

var i = 0; 
var frame = 0; 

var text = document.getElementById('text'); 

//2D Ground Grid 
var groundGridSize = 200; 
var groundDivisions = 10; 

init();

function init(){
	//Add the camera to the scene.
	scene.add(camera); 

	//Start the renderer. 
	renderer.setSize(WIDTH, HEIGHT); 

	//Attach the renderer-supplied DOM element. 
	container.appendChild(renderer.domElement); 

//	scene.add(plane); 
//	plane.rotation.x = 5; 

	//add the ground plane wireframe
	/////////////////////////
	var wireframe = new THREE.GridHelper(groundGridSize, 
		groundDivisions, 0xffffff, 0xffffff); 
	scene.add(wireframe);  
	/////////////////////

//Preferred Camera position

	camera.position.z = 110;
	camera.position.y = 70; 

	camera.rotation.x = 6; 

/*
		camera.position.z = 0;
	camera.position.y = 150; 

	camera.rotation.x = 5;
*/  
	//create a point light 
	const pointLight = new THREE.PointLight(0xFFFFFF); 

	//set its position 
	pointLight.position.x = 10; 
	pointLight.position.y = 50; 
	pointLight.position.z = 130; 

	//add to the scene
	scene.add(pointLight); 

	drawBins(); 

	//schedule the first frame 
	requestAnimationFrame(update); 

	text.innerText = "Environment" //"Plane: ( , , )"

} 

function drawBins(){
	//grid is our variable with our data 

	var groundSquareSize = groundGridSize/groundDivisions - 0.5; //account for lines 
	console.log(groundSquareSize)

	for(var x=0; x<grid.length; x++){
		//The X bin position from left to right in view range 
		var realXBinCoord = x - viewWidth/2; 

		for(var z=0; z<grid[x].length; z++){
			//The Z bin from 0 forward 

			if(grid[x][z].y != 0){
				//Cube model 
				var geometry = new THREE.BoxGeometry(groundSquareSize, 
					grid[x][z].y * groundSquareSize, groundSquareSize);
				var material = new THREE.MeshBasicMaterial({color: 0x00ff00}); 
				var cube = new THREE.Mesh(geometry, material);
				cube.material.opacity = 0.25; 
				cube.material.transparent = true;

				//Wireframe of cube  
				var wireframe = new THREE.WireframeHelper(cube, 0x00ff00); 

				//Position of cube 
				cube.position.x = realXBinCoord * groundSquareSize - 
				groundSquareSize/2; // + offsetFromGrid; 
				cube.position.y = (grid[x][z].y * groundSquareSize)/2; 
				cube.position.z =  z * (-groundSquareSize/2); //+ offsetFromGrid; 

				//Position of wireframe 
				wireframe.position.x = realXBinCoord * groundSquareSize - 
				groundSquareSize/2; // + offsetFromGrid; 
				wireframe.position.y = (grid[x][z].y * groundSquareSize)/2; 
				wireframe.position.z =  z * (-groundSquareSize/2); // + offsetFromGrid; 

				scene.add(cube); 
				scene.add(wireframe); 

				console.log("X coord: ")
				console.log(realXBinCoord)
				console.log("Z coord: ")
				console.log(z)
				console.log("grid len: ")
	console.log(grid.length)
		
			}
		}
		
	}
	
}

//TODO
//Rotating camera functionality
//https://stackoverflow.com/questions/8426822/rotate-camera-in-three-js-with-mouse
function update() {
	 
	/*
	//every _ frame, change the plane so the user can see
	if(frame % 120 == 0){
		//change plane to new value 
		var newPlane = planes[i]
		/*
		plane.position.x = newPlane.x;  
		plane.position.y = newPlane.y; 
		plane.position.z = newPlane.z; 
		
		var vectorThree = new THREE.Vector3(newPlane.x, newPlane.y,
		newPlane.z); 
		plane.normal = vectorThree;

		//Update the text display 
		text.innerText = "Plane: (" + plane.normal.x + 
		", " + plane.normal.y + ", " + plane.normal.z + 
		")" 

		i++; 

		if(planes.length <= i){
			i = 0; 
		}
	} */ 


	 
	//Draw
	renderer.render(scene, camera); 

	//schedule the next frame. 
	requestAnimationFrame(update); 
	frame++; 
}