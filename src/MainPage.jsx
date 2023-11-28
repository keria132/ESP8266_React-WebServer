/* eslint-disable react/prop-types */

import { useState, useEffect } from "react"
import partly__cloudy from './assets/partly__cloudy.svg';
import cloudy from './assets/cloudy.svg';
import sunny from './assets/sunny.svg';
import rainy from './assets/rainy.svg';
import heavy__rain from "./assets/heavy__rain.svg"
import snowfall from "./assets/snowfall.svg"
import lights__icon from "./assets/lights__icon.svg"



function WeatherWidget(props) {

    const currentWeather = props.weather;
    const cloudyPrc = currentWeather.cloudcover
    let weatherHeader, weatherStatus, weatherStatusIcon;

    //Define weather status
    //Check cloud cover
    switch(true) {
        case cloudyPrc < 10:
            weatherHeader = "Clear sky";
            weatherStatusIcon = sunny;
            break;
        case (cloudyPrc < 30 && cloudyPrc > 10):
            weatherHeader = "Mostly clear";
            weatherStatusIcon = partly__cloudy;
            break;
        case (cloudyPrc < 70 && cloudyPrc > 30):
            weatherHeader = "Partly cloudy";
            weatherStatusIcon = partly__cloudy;
            break;
        case (cloudyPrc < 80 && cloudyPrc > 70):
            weatherHeader = "Mostly cloudy";
            weatherStatusIcon = partly__cloudy;
            break;
        case (cloudyPrc > 80):
            weatherHeader = "Cloudy sky";
            weatherStatusIcon = cloudy;
            break;
        default:
            weatherHeader = "dunno init";
            break;
    }
    weatherStatus = "clear";

    //Check weather conditions
    if(currentWeather.rain > 0.5 && currentWeather.rain < 10){
        //light rain
        weatherStatus = "rainy"
        weatherStatusIcon = rainy;
    }
    if(currentWeather.rain > 10){
        //heavy rain
        weatherStatus = "heavy rain"
        weatherStatusIcon = heavy__rain;
    }

    if(currentWeather.snow > 1){
        //snowfall
        weatherStatus = "snowfall"
        weatherStatusIcon = snowfall;
    }

    return(
        <section className="weather">
            <img src={weatherStatusIcon} className="weather__icon" alt="weather__icon" />
            <div className="weather__bar">
                <h2 className="weather__header">{weatherHeader}</h2>
                <p className="weather__status">{weatherStatus}</p>
            </div>
            <b>{currentWeather.temperature_2m + "°"}</b>

            <p className="weather__values">{currentWeather.apparent_temperature + "°C"}</p>
            <p className="weather__values">{currentWeather.precipitation + "%"}</p>
            <p className="weather__values">{currentWeather.relativehumidity_2m + "%"}</p>
            <p className="weather__values">{currentWeather.windspeed_10m + " m/s"}</p>

            <p className="weather__description">Sensible</p>
            <p className="weather__description">Precipitation</p>
            <p className="weather__description">Humidity</p>
            <p className="weather__description">Wind</p>
        </section>
    )
}

function Relay (props){
    const {relay_name, relay_status, relay_ip} = props.relayData;
    const [status, setStatus] = useState(relay_status);
    console.log(props.relayData)
    

    function sendRelayStatus(){
        console.log("Sending status to: " + relay_ip);
        let url = "http://" + relay_ip + "/relaySwitch/?button_reading=" + Number(!status);
        // let statusWindow = window.open(url, "", "popup");
        // setTimeout(() => statusWindow.close(), 300);
        // setStatus(!status);

        let xhr = new XMLHttpRequest();

        xhr.open("GET", url)

        xhr.setRequestHeader('Access-Control-Allow-Origin', '*');

        xhr.onload = function() {
            console.log(`Ok: ${xhr.response}`);
            setStatus(Number(this.responseText))
            // setStatus(!status);
        };

        xhr.onerror = function() {
            console.log(`Error! \n ${xhr.status}`);
        };

        xhr.send()
        
    }

    return (
        <div className="relay">
            <h4 className="relay__header">Room {relay_name}</h4>

            <img className="relay__icon" src={lights__icon} alt="relay__icon" />

            <label className="relay__switch">

                {status == 1 ? <input type="checkbox" checked onChange={() => sendRelayStatus()} /> : <input type="checkbox" onChange={() => sendRelayStatus()} />}

                <span className="slider"></span>
            </label>

            <small>off</small>
            
        </div>
    )
}


function MainPage() {

    const [weather, setWeather] = useState({});
    const [relays, setRelays] = useState([])

    useEffect(() => {

        async function getWeather(){

            // const locationOptions = {
            //     enableHighAccuracy: true,
            // }

            // let position = await new Promise(function (resolve, reject){
            //     return navigator.geolocation.getCurrentPosition(resolve, reject, locationOptions);
            // })

            // console.log("Coords await: ", position.coords.latitude, position.coords.longitude);

            // let url ="https://api.open-meteo.com/v1/forecast?latitude=" + position.coords.latitude + "&longitude=" + position.coords.longitude + "&current=temperature_2m,relativehumidity_2m,apparent_temperature,precipitation,rain,showers,snowfall,cloudcover,windspeed_10m&windspeed_unit=ms"
            let url ="https://api.open-meteo.com/v1/forecast?latitude=" + 44.04444 + "&longitude=" + 42.86056 + "&current=temperature_2m,relativehumidity_2m,apparent_temperature,precipitation,rain,showers,snowfall,cloudcover,windspeed_10m&windspeed_unit=ms"

            console.log("Start")

            let response = await fetch(url);
            let result = await response.json()
            
            console.log("Finish")
            console.log(result);
            setWeather(result)

        }

        function getRelaysData(){

            let devices;

            let xml = new XMLHttpRequest();
            xml.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    console.log(this.responseText);
                    devices = JSON.parse(this.responseText);
                    setRelays([...devices])
                }
            }
            xml.open("GET", "/getRelaysData", true);
            xml.send();
    
        }
        
        getWeather();
        setInterval(getRelaysData, 5000);

    }, [])

    return (
        <main className="mainPage container">
            {console.log("element rendered")}

            <h1 className="mainPage__header">Hello, keria!</h1>
            <small>Welcome home</small>

            {weather.current != undefined ? <WeatherWidget weather={weather.current}/> : ""} 

            <h3 className="relaysHeader">Your devices</h3>

            <div className="relays">
                {relays.map((relay, index) => {
                    return <Relay key={index} relayData={relay}/>
                })}
            </div>
            
        </main>
    )
}

export default MainPage