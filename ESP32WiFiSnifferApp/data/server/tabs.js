/* Functions for displaying information on the tabular part of the webpage. */


function openTab( tabName, elmnt, color ) {
	
	document.getElementById("mapPage").style.display = "none";
	document.getElementById("posDataPage").style.display = "none";
	document.getElementById("measDataPage").style.display = "none";

	document.getElementById("mapTab").style.backgroundColor = "";
	document.getElementById("posTableTab").style.backgroundColor = "";
	document.getElementById("measTableTab").style.backgroundColor = "";
	
	document.getElementById( tabName ).style.display = "block";
	elmnt.style.backgroundColor = color;
	
	console.log( tabName );
	
	if ( tabName == "mapPage" ) {
		drawSetup( masterCoords, espSlaveXcoords, espSlaveYcoords, roomContourXCoords, roomContourYCoords );
	}
	
} // openTab


function appendDataToLogsTable( jsondata ) {
	
	var dataTableBody = document.getElementById("measDataTableBody");
	
	var tr = document.createElement('tr');
	tr.innerHTML =
	'<td>' + jsondata.time  + '</td>' +
	'<td>' + jsondata.MAC + '</td>';
	for ( var i = 0; i < jsondata.RSSIs.length; i++)
	{
		tr.innerHTML = tr.innerHTML +
		'<td>' + jsondata.RSSIs[i] + '</td>';
	}
	
	dataTableBody.appendChild(tr);
	
} // appendDataToLogsTable


function appendPosToPosTable( jsondata ) {
	var pos = positionNLLS( espSlaveXcoords, espSlaveYcoords, jsondata.RSSIs, RSSIc, pathLossExp );
	var x_pos = pos[0];
	var y_pos = pos[1];
	
	var posTableBody = document.getElementById("posDataTableBody");
	
	var tr = document.createElement('tr');
	tr.innerHTML = 
	'<td>' + jsondata.time  + '</td>' +
	'<td>' + jsondata.MAC + '</td>' +
	'<td>' + x_pos.toFixed(4) + '</td>' +
	'<td>' + y_pos.toFixed(4) + '</td>';
	
	posTableBody.appendChild(tr);
	
	// Draw position to the canvas on the "Map" tab.
	drawPosition( x_pos, y_pos );
	
} // appendPosToPosTable


function drawSetup( masterCoords, espSlaveXcoords, espSlaveYcoords, roomContourXCoords, roomContourYCoords ) {
	var canvas = document.getElementById("Map");
	
	if (canvas.getContext) {
		var ctx = canvas.getContext("2d");
		
		
		// Draw room contours
		ctx.lineWidth = 0.006 * ctx.canvas.width;
		ctx.lineHeight = 0.006 * ctx.canvas.height;
		ctx.beginPath();
		var contourPoint_x = ( ctx.canvas.width - 2 * edgeGapSize * ctx.canvas.width ) * ( roomContourXCoords[0] / roomMaxLength ) + edgeGapSize * ctx.canvas.width;
		var contourPoint_y = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( roomContourYCoords[0] / roomMaxWidth ) + edgeGapSize * ctx.canvas.height;

		ctx.moveTo( contourPoint_x, contourPoint_y );
		for ( var i = 1; i < roomContourXCoords.length; i++ )
		{
			var contourPoint_x = ( ctx.canvas.width - 2 * edgeGapSize * ctx.canvas.width ) * ( roomContourXCoords[i] / roomMaxLength ) + edgeGapSize * ctx.canvas.width;
			var contourPoint_y = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( roomContourYCoords[i] / roomMaxWidth ) + edgeGapSize * ctx.canvas.height;

			ctx.lineTo( contourPoint_x, contourPoint_y );
		}
		ctx.closePath();
		ctx.strokeStyle = "black";
		ctx.stroke();
		
		
		// Plot master position
		var master_x = ( ctx.canvas.width - 2 * edgeGapSize * ctx.canvas.width ) * ( masterCoords[0] / roomMaxLength ) + edgeGapSize * ctx.canvas.width;
		var master_y = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( masterCoords[1] / roomMaxWidth ) + edgeGapSize * ctx.canvas.height;
		
		ctx.beginPath();
		ctx.lineWidth = 8;
		ctx.strokeStyle = "green";
		ctx.rect( master_x - 28, master_y - 28, 60, 60 );
		ctx.stroke();
		
		ctx.font = "bold 48px Arial";
		ctx.fillStyle = "green";
		ctx.fillText("M", master_x - 18, master_y + 17 );
		
		if ( demo ) {
			
			console.log( demo );
			
			var dist_vertex_to_midpoint = 30;
			
			var demo_device_x = ( ctx.canvas.width - 2 * edgeGapSize * ctx.canvas.width ) * ( demo_device_position[0] / roomMaxLength ) + edgeGapSize * ctx.canvas.width;
			var demo_device_y = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( demo_device_position[1] / roomMaxWidth ) + edgeGapSize * ctx.canvas.height;
			
			console.log( demo_device_x );
			console.log( demo_device_y );
			
			ctx.beginPath();
			ctx.lineWidth = 8;
			ctx.moveTo( demo_device_x, demo_device_y - 30 );
			ctx.lineTo( demo_device_x - 30 * Math.cos(30 * Math.PI / 180), demo_device_y + 30 * Math.sin(30 * Math.PI / 180) );
			ctx.lineTo( demo_device_x + 30 * Math.cos(30 * Math.PI / 180), demo_device_y + 30 * Math.sin(30 * Math.PI / 180) );
			ctx.closePath();
			ctx.closePath();
			ctx.strokeStyle = "black";
			ctx.stroke();
			
			ctx.fillStyle = "yellow";
			ctx.fill();
			
		}
		
		
		// Plot slave positions
		for ( var i = 0; i < espSlaveXcoords.length; i++ )
		{
			var slave_x = ( ctx.canvas.width - 2 * edgeGapSize * ctx.canvas.width ) * ( espSlaveXcoords[i] / roomMaxLength ) + edgeGapSize * ctx.canvas.width;
			var slave_y = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( espSlaveYcoords[i] / roomMaxWidth ) + edgeGapSize * ctx.canvas.height;
			
			console.log( slave_x );
			console.log( slave_y );
			
			ctx.beginPath();
			ctx.lineWidth = 8;
			ctx.strokeStyle = "blue";
			ctx.rect( slave_x - 28, slave_y - 35, 60, 60 );
			ctx.stroke();
			
			ctx.font = "bold 48px Arial";
			ctx.fillStyle = "blue";
			ctx.fillText( String(i+1), slave_x - 12, slave_y + 14 );
		}		
	}
	else {
		alert("Oh boy.. your browser is dumb..");
	}
	
} // drawRoom


function drawPosition( x, y ) {
	
	var ctx = document.getElementById("Map").getContext("2d");
	var pointSize = 0.005 * ctx.canvas.width;
	
	var x_pos = ( ctx.canvas.width -  2 * edgeGapSize * ctx.canvas.width ) * ( x / roomMaxLength ) +
			      edgeGapSize * ctx.canvas.width;
	var y_pos = ( ctx.canvas.height - 2 * edgeGapSize * ctx.canvas.height ) * ( y / roomMaxWidth ) +
				  edgeGapSize * ctx.canvas.height;
	
	ctx.fillStyle = "#ff2626";
	ctx.beginPath(); //Start path
    ctx.arc( x_pos, y_pos, pointSize, 0, Math.PI * 2, true); // Draw a point using the arc function of the canvas with a point structure.
    ctx.fill(); // Close the path and fill.
	
} // drawPosition