<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "environment_monitor";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $temperature = isset($_POST['temperature']) ? floatval($_POST['temperature']) : 0;
    $humidity = isset($_POST['humidity']) ? floatval($_POST['humidity']) : 0;
    $co2 = isset($_POST['co2']) ? intval($_POST['co2']) : 0;
    $air_quality = isset($_POST['air_quality']) ? $_POST['air_quality'] : '';

    if ($temperature && $humidity && $co2 && $air_quality) {
        $stmt = $conn->prepare("INSERT INTO sensor_data (timestamp, temperature, humidity, co2, air_quality) VALUES (CURRENT_TIMESTAMP, ?, ?, ?, ?)");
        $stmt->bind_param("ddis", $temperature, $humidity, $co2, $air_quality);
        
        if ($stmt->execute()) {
            echo "New record created successfully";
        } else {
            echo "Error: " . $stmt->error;
        }

        $stmt->close();
    } else {
        echo "Invalid input data";
    }
} else {
    echo "Invalid request method";
}

$conn->close();
?>
