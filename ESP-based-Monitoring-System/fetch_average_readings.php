<?php
header('Content-Type: application/json');
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "environment_monitor";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die(json_encode(array("error" => "Connection failed: " . $conn->connect_error)));
}

// SQL query to fetch the average readings
$sql = "SELECT average_temperature, average_co2, timestamp FROM average_readings ORDER BY timestamp DESC LIMIT 10";
$result = $conn->query($sql);

// Initialize an array to store the result
$averageReadings = array();

if ($result->num_rows > 0) {
    // Fetch each row and add it to the array
    while($row = $result->fetch_assoc()) {
        $averageReadings[] = $row;
    }
} else {
    echo json_encode(array("error" => "No data found"));
    exit();
}

// Return the result as a JSON array
echo json_encode($averageReadings);

// Close connection
$conn->close();
?>


