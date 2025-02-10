function showNotification(message, isError) {
    const notification = document.getElementById('notification');
    const notificationText = document.getElementById('notification-text');

    notificationText.textContent = message;
    notification.classList.remove('hidden');

    if (isError) {
        notification.classList.add('error');
    } else {
        notification.classList.remove('error');
    }

    setTimeout(() => { notification.classList.add('hidden'); }, 5000);
}

module.exports = { showNotification };