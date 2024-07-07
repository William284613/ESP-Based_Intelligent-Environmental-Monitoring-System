<?php

// Database configuration
$servername = "localhost"; // Database server name
$username = "root"; // Database username
$password = ""; // Database password
$dbname = "environment_monitor"; // Database name

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Fetch last readings
$sql = "SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT 10";
$result = $conn->query($sql);

$readings = array();
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $readings[] = $row;
    }
} else {
    error_log("No readings found");
}

$conn->close();

// Return JSON response
header('Content-Type: application/json');
echo json_encode($readings, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);

// Debugging: Log the JSON response
error_log(json_encode($readings, JSON_UNESCAPED_SLASHES | JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE));
?>
