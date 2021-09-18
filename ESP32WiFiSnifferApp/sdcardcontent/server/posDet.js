/* Functions for determining position based on RSSI values. */


/*
 Returns the square of the distance between esp and device
 estimated by the log-normal shadowing model (lnsm).
 */
function lnsmDistanceEstimateSquared( rssi, one_meter_rssi, path_loss_exponent ) {
    return Math.pow( 10, ( 2 * (one_meter_rssi - rssi) / (10 * path_loss_exponent) ) );
    
} // lnsmDistanceEstimateSquared


/*
 Returns the eucledian distnace squared of the device and the esp.
 */
function distanceSquared( x_0, y_0, x_esp, y_esp ) {
    return Math.pow( (x_0 - x_esp), 2 ) + Math.pow( (y_0 - y_esp), 2 );
    
} // distanceSquared


/*
 Returns the eucledian norm of a vector.
 */
function normOfVector( vector ) {
    var norm = 0;
    
    for ( var i = 0; i < vector.length; i++ )
    {
        norm += ( vector[i] * vector[i] );
    }
    
    return Math.sqrt( norm );
        
} // normOfVector


/*
 Returns the difference of squares of the lnsm-estimated distance from esps
 and the euclidien distance of the current calculated position from esps .
 */
function calculateErrorVector( x_0, y_0, rssis, x_esps, y_esps, one_meter_rssi, path_loss_exponent ) {
    var errorVect = [];
    for ( var i = 0; i < x_esps.length; i++ )
    {
        errorVect[i] = lnsmDistanceEstimateSquared( rssis[i], one_meter_rssi, path_loss_exponent)
                       - distanceSquared( x_0, y_0, x_esps[i], y_esps[i] );
    }
    
    return errorVect;
    
} // calculateErrorVector


/*
 Calculates the Jacobian of the eucledian distance function
 evaluated for the given device and esp positions.
 */
function jacobian( x_0, y_0, x_esps, y_esps) {
    const m = x_esps.length;
    const n = 2;
    
    var J = Array.apply(null, Array(m)); // create an empty array of length n
    for (var i = 0; i < m; i++) {
      J[i] = Array.apply(null, Array(n)); // make each element an array
    }
    
    for ( var i = 0; i < x_esps.length; i++ )
    {
        J[i][0] = 2 * ( x_0 - x_esps[i] );
        J[i][1] = 2 * ( y_0 - y_esps[i] );
    }
    
    /*
    console.log("J calculated");
    for ( var i=0; i < J.length; i++ )
    {
        console.log( J[i] );
    }
    console.log(J.length);
    */
    
    return J;
}


function transpJtimesJ( J ) {

    tJJ = [[0, 0],[0, 0]];

    for ( var i = 0; i < J.length; i++ )
    {
        tJJ[0][0] += J[i][0] * J[i][0];
        tJJ[0][1] += J[i][0] * J[i][1];
        tJJ[1][0] += J[i][1] * J[i][0];
        tJJ[1][1] += J[i][1] * J[i][1];
    }
    
    return tJJ;		
    
} // transpJtimesJ


function transpJtimesVect( J, v ) {
    
    var tJv = [0, 0];
    
    for ( var i = 0; i < v.length; i++ )
    {
        tJv[0] += J[i][0] * v[i];
        tJv[1] += J[i][1] * v[i];
    }
    
    return tJv;	
    
} // transpJtimesVect


/*
 The initial position guess for the non-linear least squares iteration is taken
 as the position of the closest esp device based on the lnsm-estimated distance.
 */
function initPosGuess( x_esps, y_esps, rssis, one_meter_rssi, path_loss_exponent ) {
    var lnsmDistSquares = [];	
    
    for ( var i = 0; i < x_esps.length; i++ )
    {
        lnsmDistSquares[i] = lnsmDistanceEstimateSquared( rssis[i], one_meter_rssi, path_loss_exponent );
    }
    
    var indexOfMin = lnsmDistSquares.indexOf( Math.min(...lnsmDistSquares) );
    
    var iPG = [ x_esps [indexOfMin], y_esps[indexOfMin] ];
    
    return iPG;
    
} // initPosGuess


/*
 Gauss solver: direct equation system solver. Since ours is only a 2x2 system
 (we are looking for 2 position values, which minimise the error in least squares sense)
 the bad complexity of the solver shouldn't be a huge issue.
 Still, it could be changed for something more sophisticated...
 */
function gaussSolver( A, b ) {
    var len = b.length;
    var deltaPos = [0, 0];
    var toleranceForZero = 0.000001;
    
    var absOfDetA = Math.abs( A[0][0] * A[1][1] - A[0][1]*A[1][0] );
    
    if ( absOfDetA < 0.01 )
    {
        console.log( "Determinant is zero" );
    }
    
    for ( var k = 0; k < (len-1); k++ )
    {
        for ( var i = (k+1); i < len; i++ )
        {
            if ( A[k][k] > toleranceForZero )
            {
                A[i][k] = A[i][k] / A[k][k];
            }
            else
            {
                console.log("Pivot is zero!");
            }
            for ( var j = (k+1); j < len; j++)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }
    
    for ( var k = 1; k < len; k++)
    {
        for ( var i = 0; i < (k-1); i++ )
        {
            b[k] = b[k] - A[k][i] * b[i];
        }
    }
    
    for ( var k = (len-1); k >= 0; k-- )
    {
        for ( var i = (k+1); i < len; i++ )
        {
            b[k] = b[k] - A[k][i] * deltaPos[i];
        }
        deltaPos[k] = b[k] / A[k][k];
    }
    
    return deltaPos;
    
} // gaussSolver


function vectorAdd( a, b ) {
    
    if ( a.length == b.length )
    {
        var sum = [];
        for ( var i = 0; i < a.length; i++ )
        {
            sum[i] = a[i] + b[i];
        }
    }

    return sum;
    
} // vectorAdd

/*
 Non-linear leas squares iteration for determining the unknown position (c_0) of a device
 based on RSSi values measured by the esps.
 It solves the equation: transp(J)*J*deltaC_0 = transp(J)*R for each itaration.
 */
function positionNLLS( x_esps, y_esps, rssis, one_meter_rssi, path_loss_exponent ) {
    
    var errorTolerance = 0.01;
    var c_0 = initPosGuess( x_esps, y_esps, rssis, one_meter_rssi, path_loss_exponent );
    console.log(c_0);
    var R = calculateErrorVector( c_0[0], c_0[1], rssis, x_esps, y_esps, one_meter_rssi, path_loss_exponent );
    
    var X = [[0, 0],[0, 0]];
    var b = [0, 0];
    var deltaC_0 = [0, 0];
    
    console.log( "New position iteration has started: "); 
    
    for ( var i = 0; i < 10; i++ )
    {
        J = jacobian( c_0[0], c_0[1], x_esps, y_esps, rssis, one_meter_rssi, path_loss_exponent );
        //console.log( J );
        X = transpJtimesJ( J );
        //console.log( X );
        b = transpJtimesVect( J, R );
        //console.log( b );
        deltaC_0 = gaussSolver( X, b );
        console.log(deltaC_0);
        c_0 = vectorAdd( c_0, deltaC_0);
        console.log(c_0);
        R = calculateErrorVector( c_0[0], c_0[1], rssis, x_esps, y_esps, one_meter_rssi, path_loss_exponent );
        console.log(R);
    }
    
    return c_0;	
    
} // positionNLLS