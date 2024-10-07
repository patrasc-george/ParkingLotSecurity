const nameInput = document.getElementById("name");
const passwordInput = document.getElementById("password");
const errorMessageElement = document.getElementById("errorMessage");

loginButton.addEventListener("click", function () {
    const name = nameInput.value;
    const password = passwordInput.value;

    errorMessageElement.textContent = '';

    if (!name || !password) {
        errorMessageElement.textContent = "Please enter both name and password.";
        return;
    }

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('name', name);
    urlEncodedData.append('password', password);

    fetch('http://localhost:8080/api/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: urlEncodedData
    })
    .then(response => response.json())
    .then(data => handleServerResponse(data))
    .catch(error => {
        console.error('Error:', error);
        errorMessageElement.textContent = 'An error occurred. Please try again later.';
    });
});

function handleServerResponse(data) {
    console.log('Server response:', data);

    setTimeout(() => {
        if (data.success === false) {
            errorMessageElement.textContent = data.message || 'Login failed. Please check your credentials.';
            errorMessageElement.style.display = 'block';
        } else if (data.success === true) {
            errorMessageElement.style.display = 'none';
            if (data.subscriptionsTable) {
                localStorage.setItem('subscriptionsTable', JSON.stringify(data.subscriptionsTable));
            }
            localStorage.setItem('subscriptionsData', JSON.stringify(data.subscriptions));
            localStorage.setItem('name', nameInput.value);
            window.location.href = 'subscriptions.html';
        } else {
            errorMessageElement.textContent = 'An unexpected response was received. Please try again.';
            errorMessageElement.style.display = 'block';
        }
    }, 1000);
}
