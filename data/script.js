function loadPage(path, scriptSrcs = []) {
    fetch(path) // Загрузка HTML
        .then(response => {
            if (!response.ok) {
                throw new Error(`Failed to load ${path}: ${response.statusText}`);
            }
            return response.text(); // Получение HTML-текста
        })
        .then(html => {
            const content = document.getElementById('content');

            // Загрузка нового HTML-контента
            content.innerHTML = html;

            // Загрузка внешних скриптов
            scriptSrcs.forEach(scriptSrc => {
                const scriptElement = document.createElement('script');
                scriptElement.src = scriptSrc; // Путь к скрипту
                scriptElement.async = false; // Скрипты выполняются по порядку
                document.body.appendChild(scriptElement); // Добавление в DOM
            });
        })
        .catch(error => {
            console.error(`Error loading page ${path}:`, error);
        });
}

// Пример использования
loadPage('/myPage.html', ['/path/to/script1.js', '/path/to/script2.js']);


document.addEventListener('DOMContentLoaded', () => {
    const wifiButton = document.getElementById('wifiButton');
    const bleButton = document.getElementById('bleButton');

    wifiButton.addEventListener('click', () => loadPage('/wifi.html', ['/wifi.js'])); // Загрузка страницы Wi-Fi с дополнительным скриптом
    bleButton.addEventListener('click', () => loadPage('/ble.html', ['/ble.js'])); // Загрузка страницы BLE со скриптом
});
