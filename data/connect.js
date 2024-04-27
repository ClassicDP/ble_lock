document.addEventListener('DOMContentLoaded', () => {
    const networkName = getQueryParameter('network'); // Имя сети из URL
    const networkInfo = document.getElementById('networkInfo'); // Элемент для отображения имени сети
    const networkElement = document.getElementById('network'); // Скрытое поле с именем сети
    const statusElement = document.getElementById('connectionStatus'); // Элемент для вывода статуса

    if (networkName) {
        networkElement.value = networkName; // Установить имя сети
        networkInfo.textContent += networkName; // Обновить информацию о сети
    }

    const form = document.getElementById('connectForm'); 
    form.addEventListener('submit', event => {
        event.preventDefault(); // Предотвратить стандартное поведение формы

        const formData = new FormData(form);

        fetch('/connect', {
            method: 'POST',
            body: formData
        })
        .then(response => response.json()) // Получить JSON-ответ
        .then(data => {
            if (data.status === 'success') {
                const ip = data.ip; // IP-адрес после подключения
                statusElement.textContent = `Connected! IP Address: ${ip}`; // Успешное подключение
            } else {
                statusElement.textContent = 'Connection failed. Try again.'; // Подключение не удалось
            }
        })
        .catch(error => {
            console.error('Error during connection:', error); // Логирование ошибки
            statusElement.textContent = 'An error occurred. Please try again.'; // Общее сообщение об ошибке
        });
    });
});

// Функция для получения параметра из URL
function getQueryParameter(name) {
    const params = new URLSearchParams(window.location.search);
    return params.get(name);
}
