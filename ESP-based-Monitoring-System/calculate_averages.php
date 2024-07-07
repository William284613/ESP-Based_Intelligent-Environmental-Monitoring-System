<?php
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

// Fetch the last 10 readings
$sql = "SELECT temperature, co2 FROM sensor_data ORDER BY timestamp DESC LIMIT 10";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    $totalTemperature = 0;
    $totalCO2 = 0;

    // Calculate the sum of temperature and CO2 values
    while ($row = $result->fetch_assoc()) {
        $totalTemperature += $row["temperature"];
        $totalCO2 += $row["co2"];
    }

    // Calculate the average values
    $averageTemperature = $totalTemperature / 10;
    $averageCO2 = $totalCO2 / 10;

    // Insert the average values into the average_readings table
    $insertSql = "INSERT INTO average_readings (average_temperature, average_co2) VALUES ($averageTemperature, $averageCO2)";
    
    if ($conn->query($insertSql) === TRUE) {
        echo "New average values inserted successfully";
    } else {
        echo "Error: " . $insertSql . "<br>" . $conn->error;
    }
} else {
    echo "No readings found";
}

$conn->close();
?>
