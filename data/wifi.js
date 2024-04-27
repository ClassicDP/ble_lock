document.addEventListener('DOMContentLoaded', () => {
    const refreshButton = document.querySelector('#refreshButton');

    refreshButton.addEventListener('click', getWiFiNetworks); // Обновление списка сетей

    const wifiTable = document.querySelector('#wifiTable tbody'); // Таблица с сетями

    // Пример обработки нажатия на кнопку "Connect"
    wifiTable.addEventListener('click', event => {
        if (event.target.tagName.toLowerCase() === 'button') { // Проверка клика на кнопку
            const ssid = event.target.getAttribute('data-ssid'); // Имя сети
            window.location.href = `/connect.html?network=${encodeURIComponent(ssid)}`; // Переход на страницу подключения
        }
    });

    getWiFiNetworks(); // Получение списка сетей при загрузке страницы
});

function getWiFiNetworks() {
    // Функция для загрузки списка доступных сетей
    fetch('/networks')
        .then(response => response.json())
        .then(data => {
            const wifiTableBody = document.querySelector('#wifiTable tbody'); // Получение тела таблицы
            wifiTableBody.innerHTML = ''; // Очистка содержимого таблицы

            data.networks.forEach(network => {
                const row = document.createElement('tr'); // Создание новой строки
                const ssidCell = document.createElement('td'); 
                ssidCell.textContent = network.ssid; // Имя сети

                const rssiCell = document.createElement('td');
                rssiCell.textContent = network.rssi; // Уровень сигнала

                const connectButton = document.createElement('button'); // Кнопка подключения
                connectButton.textContent = 'Connect';
                connectButton.setAttribute('data-ssid', network.ssid); // Добавление имени сети в атрибут

                const buttonCell = document.createElement('td'); 
                buttonCell.appendChild(connectButton); // Добавление кнопки в ячейку

                row.appendChild(ssidCell);
                row.appendChild(rssiCell);
                row.appendChild(buttonCell); // Добавление ячеек в строку

                wifiTableBody.appendChild(row); // Добавление строки в таблицу
            });
        })
        .catch(error => {
            console.error("Error loading Wi-Fi networks:", error);
        });
}
