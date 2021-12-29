var container = document.querySelector("#container");
var scanBtn = document.querySelector("#scanBtn");
var cover = document.querySelector(".cover");
var ssidInput = document.querySelector("#ssid");
var children = container.childNodes;

async function fetchAsync(url) {
    let response = await fetch(url);
    let data = await response.json();
    return data;
}

data = fetchAsync("http://localhost:1373/");
dataList = []
data.then(d => {
    dataList = d
    d.forEach(element => {
        if (element === "") return
        var div = document.createElement("div");
        div.className = "item_container";
        div.id = element;
        div.innerHTML = element;
        div.onclick = onElementClick
        div.innerHTML += '<img class="wifi-icon" src="/wifi_icon.svg" alt="wifi">';
        container.appendChild(div);
    });
})

scanBtn.addEventListener('click', () => {
    let data = fetchAsync("http://localhost:1373/");
    let height = container.scrollHeight;
    cover.style.height = `${height}px`;
    cover.classList.add("uncoverit");
    cover.classList.remove("coverit");

    data.then(d => {
        const filteredList = d.filter(value => !dataList.includes(value))
        dataList = d
        if (filteredList.length > 0) {
            filteredList.forEach(element => {
                var div = document.createElement("div");
                div.className = "item_container";
                div.id = element;
                div.onclick = onElementClick
                div.innerHTML = element;
                div.innerHTML += '<img class="wifi-icon" src="/wifi_icon.svg" alt="wifi">';
                container.insertBefore(div, children[3])
            })
            container.scrollTop = 0;
        }
        cover.classList.remove("uncoverit");
        cover.classList.add("coverit");

    }).catch(e => {
        cover.classList.remove("uncoverit");
        cover.classList.add("coverit");
    })
})
const onElementClick = e => {
    let ssid = e.target.id;
    ssidInput.value = ssid.trim();
}    