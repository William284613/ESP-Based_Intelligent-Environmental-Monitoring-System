const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Environmental Monitoring System</title>
    <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;700&display=swap" rel="stylesheet">
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.3/dist/chart.umd.min.js"></script>
    <style>
        :root {
            --primary-color: #3498db;
            --secondary-color: #2ecc71;
            --background-color: #f4f6f9;
            --text-color: #333;
            --card-background: #ffffff;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            font-family: 'Roboto', sans-serif;
            background-color: var(--background-color);
            color: var(--text-color);
            line-height: 1.6;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            text-align: center; 
        }

        header {
            background-color: var(--primary-color);
            color: white;
            text-align: center;
            padding: 20px 0;
            margin-bottom: 30px;
        }

        h1 {
            font-size: 2.5rem;
            margin-bottom: 10px;
        }

        h2 {
            font-size: 2.0rem;
            color: var(--primary-color);
            margin-bottom: 20px;
        }

        .dashboard {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }

        .card {
            background-color: var(--card-background);
            border-radius: 8px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            padding: 20px;
            text-align: center;
        }

        .card i {
            font-size: 3rem;
            margin-bottom: 10px;
        }

        .card-title {
            font-size: 1.2rem;
            font-weight: bold;
            margin-bottom: 10px;
        }

        .card-value {
            font-size: 2rem;
            font-weight: bold;
            color: var(--primary-color);
        }

        .units {
            font-size: 1rem;
            color: #666;
        }

        button {
            background-color: var(--primary-color);
            align-items: center;
            color: white;
            border: none;
            padding: 10px 20px;
            font-size: 1rem;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }

        button:hover {
            background-color: #2980b9;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 30px;
            background-color: var(--card-background);
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }

        th,
        td {
            padding: 12px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }

        th {
            background-color: var(--primary-color);
            color: white;
        }

        .chart-container {
            background-color: var(--card-background);
            border-radius: 8px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            padding: 20px;
            margin-bottom: 30px;
        }

        @media (max-width: 768px) {
            .dashboard {
                grid-template-columns: 1fr;
            }
        }
    </style>
</head>

<body>
    <header>
        <h1>ESP-Based Intelligent Environmental Monitoring System</h1>
        <p>Final Project - SKIH3113 Sensor Based System (A232)</p>
    </header>

    <div class="container">
        <button onclick="activateSystem()">Activate System</button>

        <div class="dashboard">
            <div class="card">
                <i class="fas fa-thermometer-half" style="color:#62a1d3;"></i>
                <div class="card-title">Temperature</div>
                <div class="card-value"><span id="TemperatureValue">0</span><span class="units">&deg;C</span></div>
            </div>
            <div class="card">
                <i class="fas fa-tint" style="color:#75e095;"></i>
                <div class="card-title">Humidity</div>
                <div class="card-value"><span id="HumidityValue">0</span><span class="units">%</span></div>
            </div>
            <div class="card">
                <i class="fas fa-smog" style="color:#e3a8c7;"></i>
                <div class="card-title">Gas Concentration</div>
                <div class="card-value"><span id="GasValue">0</span><span class="units">ppm</span></div>
            </div>
            <div class="card">
                <i class="fas fa-wind" style="color:#f7dc68;"></i>
                <div class="card-title">Air Quality</div>
                <div class="card-value"><span id="AirQuality">0</span></div>
            </div>
        </div>

        <div class="card">
            <i class="far fa-clock"></i>
            <span id="time"></span>
            <i class="far fa-calendar-alt"></i>
            <span id="date"></span>
        </div>

        <h2>Average Value to Trigger Relay</h2>
        <ul>
            <li>AUTO MODE: Average Temperature (&deg;C) >= 24 OR CO2 AVERAGE (ppm) >= 500</li>
            <li>MANUAL MODE TO CHANGE THE RELAY MANUALLY</li>
            <li>The Auto mode Condition indicates an Air Circulation and Pollution Problem.</li>
        </ul>

        <h2>System Status</h2>
        <table>
            <thead>
                <tr>
                    <th>Temp AVERAGE (&deg;C)</th>
                    <th>CO2 AVERAGE (ppm)</th>
                    <th>RELAY STATUS</th>
                    <th>MODE</th>
                </tr>
            </thead>
            <tbody id="average-table">
            </tbody>
        </table>

        <h2>Last 10 Readings</h2>
        <table>
            <thead>
                <tr>
                    <th>Temperature (&deg;C)</th>
                    <th>Humidity (%)</th>
                    <th>CO2 (ppm)</th>
                    <th>Air Quality</th>
                    <th>Timestamp</th>
                </tr>
            </thead>
            <tbody id="data-table">
            </tbody>
        </table>

        <h2>Average Temperature and CO2</h2>
        <div class="chart-container">
            <canvas id="temperatureChart"></canvas>
        </div>
        <div class="chart-container">
            <canvas id="co2Chart"></canvas>
        </div>
    </div>
    <script>

        // Global variables to hold chart instances
        var temperatureChart;
        var co2Chart;

        // Function to fetch all sensor data periodically
        setInterval(function () {
            getAllSensorData();
            fetchData();
            fetchAverageData();
        }, 3000); // Fetch data every 3 seconds

        // Function to get all sensor data
        function getAllSensorData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var sensorData = this.responseText.split(',');
                    document.getElementById("TemperatureValue").innerHTML = sensorData[0];
                    document.getElementById("HumidityValue").innerHTML = sensorData[1];
                    document.getElementById("GasValue").innerHTML = sensorData[2];
                    document.getElementById("AirQuality").innerHTML = sensorData[3];
                }
            };
            xhttp.open("GET", "/readAllSensors", true);
            xhttp.send();
        }

        // Function to fetch last 10 readings
        function fetchData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    console.log("Response Text:", this.responseText); // Log the response text
                    try {
                        var data = JSON.parse(this.responseText); // Attempt to parse JSON
                        var table = document.getElementById("data-table");
                        table.innerHTML = "";
                        for (var i = 0; i < data.length; i++) {
                            var row = `<tr>
            <td>${data[i].temperature}</td>
            <td>${data[i].humidity}</td>
            <td>${data[i].co2}</td>
            <td>${data[i].air_quality}</td>
            <td>${data[i].timestamp}</td>
          </tr>`;
                            table.innerHTML += row;
                        }
                    } catch (error) {
                        console.error("JSON Parse Error:", error);
                    }
                }
            };
            xhttp.open("GET", "/fetchLastReadings", true);
            xhttp.send();
        }

        function fetchAverageData() {
            fetch('/fetchAverageReadings')
                .then(response => response.json())
                .then(data => {
                    if (Array.isArray(data) && data.length > 1) {
                        const graphData = data.slice(0, -1);
                        const statusData = data[data.length - 1];

                        const lastDataPoint = graphData[0]; // Most recent data point
                        const lastAverageTemp = parseFloat(lastDataPoint.average_temperature);
                        const lastAverageCO2 = parseFloat(lastDataPoint.average_co2);
                        const timestamp = lastDataPoint.timestamp;

                        document.getElementById("average-table").innerHTML = `
          <tr>
            <td>${lastAverageTemp}</td>
            <td>${lastAverageCO2}</td>
            <td><button id="relayButton" onclick="toggleRelay()">${statusData.relay}</button></td>
            <td><button id="modeButton" onclick="toggleMode()">${statusData.mode}</button></td>
          </tr>
        `;

                        temperatureChart = updateOrCreateChart(temperatureChart, 'temperatureChart', lastAverageTemp, timestamp);
                        co2Chart = updateOrCreateChart(co2Chart, 'co2Chart', lastAverageCO2, timestamp);
                    }
                })
                .catch(error => console.error('Error:', error));
        }

        let isAutoMode = true;
        let relayStatus = "OFF";

        function toggleMode() {
            isAutoMode = !isAutoMode;
            updateModeAndRelay();
        }

        function toggleRelay() {
            if (!isAutoMode) {
                relayStatus = relayStatus === "ON" ? "OFF" : "ON";
                updateModeAndRelay();
            }
        }

        function updateModeAndRelay() {
            fetch(`/updateMode?auto=${isAutoMode}&relay=${relayStatus}`)
                .then(response => {
                    if (!response.ok) {
                        throw new Error(`HTTP error! status: ${response.status}`);
                    }
                    return response.text();
                })
                .then(text => {
                    let data;
                    try {
                        data = JSON.parse(text);
                        document.getElementById("modeButton").textContent = data.mode;
                        document.getElementById("relayButton").textContent = data.relay;
                    } catch (e) {
                        console.error("Failed to parse JSON:", text);
                        // Handle unexpected sensor data
                        if (text.split(',').length === 4) {
                            console.log("Received sensor data instead of mode/relay status");
                            // You could update sensor displays here if needed
                        } else {
                            throw new Error("Invalid response format");
                        }
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    // Show an error message to the user
                    alert("An error occurred while updating the mode/relay status. Please try again.");
                });
        }

        function updateOrCreateChart(chartInstance, chartId, newData, newLabel) {
            const ctx = document.getElementById(chartId).getContext('2d');

            if (chartInstance) {
                // Update existing chart
                chartInstance.data.labels.push(newLabel);
                chartInstance.data.datasets[0].data.push(newData);

                // Remove old data if we have more than 10 points
                if (chartInstance.data.labels.length > 10) {
                    chartInstance.data.labels.shift();
                    chartInstance.data.datasets[0].data.shift();
                }

                chartInstance.update();
            } else {
                // Create new chart
                chartInstance = new Chart(ctx, {
                    type: 'line',
                    data: {
                        labels: [newLabel],
                        datasets: [{
                            label: chartId === 'temperatureChart' ? 'Temperature (°C)' : 'CO2 (ppm)',
                            data: [newData],
                            borderColor: chartId === 'temperatureChart' ? 'rgba(75, 192, 192, 1)' : 'rgba(255, 99, 132, 1)',
                            borderWidth: 1,
                            fill: false
                        }]
                    },
                    options: {
                        scales: {
                            x: {
                                beginAtZero: true,
                                ticks: {
                                    maxRotation: 0,
                                    maxTicksLimit: 10
                                }
                            },
                            y: { beginAtZero: true }
                        },
                        animation: {
                            duration: 0 // general animation time
                        },
                        hover: {
                            animationDuration: 0 // duration of animations when hovering an item
                        },
                        responsiveAnimationDuration: 0 // animation duration after a resize
                    }
                });
            }

            return chartInstance;
        }


        // Function to update an existing chart with new data
        function updateChart(chart, labels, data) {
            chart.data.labels = labels;
            chart.data.datasets[0].data = data;
            chart.update();
        }

        // Function to plot temperature chart
        function plotTemperatureChart(labels, temperatureData) {
            var temperatureChartCtx = document.getElementById('temperatureChart').getContext('2d');
            return new Chart(temperatureChartCtx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Average Temperature (°C)',
                        data: temperatureData,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                },
                options: {
                    scales: {
                        x: {
                            beginAtZero: true
                        },
                        y: {
                            beginAtZero: true
                        }
                    }
                }
            });
        }

        // Function to plot CO2 chart
        function plotCO2Chart(labels, co2Data) {
            var co2ChartCtx = document.getElementById('co2Chart').getContext('2d');
            return new Chart(co2ChartCtx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Average CO2 (ppm)',
                        data: co2Data,
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                },
                options: {
                    scales: {
                        x: {
                            beginAtZero: true
                        },
                        y: {
                            beginAtZero: true
                        }
                    }
                }
            });
        }

        // Function to update time and date
        setInterval(function () {
            Time_Date();
        }, 1000); // Update time and date every second

        function Time_Date() {
            var t = new Date();
            document.getElementById("time").innerHTML = t.toLocaleTimeString(); // Display current time
            const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
            const monthNames = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];
            document.getElementById("date").innerHTML = dayNames[t.getDay()] + ", " + t.getDate() + "-" + monthNames[t.getMonth()] + "-" + t.getFullYear(); // Display current date
        }

        //function to activate system
        function activateSystem() {
            fetch('/activateSystem')
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                    alert('System activated!');
                })
                .catch((error) => {
                    console.error('Error:', error);
                    alert('Failed to activate system');
                });
        }

    </script>

</body>

</html>
)=====";