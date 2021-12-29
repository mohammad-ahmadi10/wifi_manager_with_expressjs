const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const PORT = 1373
const app = express();

app.use(require('access-control')({ /* options here */ }));
app.use(express.json())
app.use(bodyParser.urlencoded({ extended: true }));
const { spawnSync } = require('child_process');



var allowlist = ['http://192.168.1.22', 'http://192.168.1.24', "http://localhost:3000"]
var corsOptionsDelegate = function (req, callback) {
    var corsOptions;
    if (allowlist.indexOf(req.header('Origin')) !== -1) {
        corsOptions = { origin: true } // reflect (enable) the requested origin in the CORS response
    } else {
        corsOptions = { origin: false } // disable CORS for this request
    }
    callback(null, corsOptions) // callback expects two parameters: error and options
}

let wlanslist = "";

refreshData = () => {
    spawnSync("WlanScan", { encoding: 'utf8' });
    let child = spawnSync("netsh", ['wlan', 'show', 'networks'], { encoding: 'utf8' })

    wlanslist = child.stdout
    wlanslist = [...wlanslist.split("\r")]

    wlanslist = wlanslist.filter(element => {
        if (element.includes("SSID")) {
            element.trim();
            return element
        }
    });
    wlanslist = wlanslist.map(element => element.trim().slice(9));
}
app.get("/", cors(corsOptionsDelegate), (req, res) => {
    refreshData();
    res.send(wlanslist)
})
app.post("/data", (req, res) => {
    console.log(req.body);
    res.send(JSON.stringify(req.body))
})

app.listen(process.env.PORT || PORT, _ => console.log(`listing at port: ${PORT}`));