const createButton = document.getElementById("createButton");
const nameInput = document.getElementById("name");
const passwordInput = document.getElementById("password");
const confirmPasswordInput = document.getElementById("confirmPassword");
const errorMessageElement = document.getElementById("errorMessage");

createButton.addEventListener("click", function () {
    const name = nameInput.value.trim();
    const password = passwordInput.value;
    const confirmPassword = confirmPasswordInput.value;

    errorMessageElement.textContent = '';

    if (!name || !password || !confirmPassword) {
        errorMessageElement.textContent = "Please fill in all fields.";
        return;
    }

    if (!validateEmail(name)) {
        errorMessageElement.textContent = "Please enter a valid email address.";
        return;
    }

    if (password !== confirmPassword) {
        errorMessageElement.textContent = "Passwords do not match.";
        return;
    }

    const urlEncodedData = new URLSearchParams();
    urlEncodedData.append('name', name);
    urlEncodedData.append('password', password);

    fetch('http://localhost:8080/api/createSubscription', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: urlEncodedData
    })
    .then(response => response.json())
    .then(data => handleServerResponse(data, name))
    .catch(error => {
        console.error('Error:', error);
        errorMessageElement.textContent = 'An error occurred. Please try again later.';
    });
});

function handleServerResponse(data, name) {
    console.log('Server response:', data);

    setTimeout(() => {
        if (data.success === false) {
            errorMessageElement.textContent = data.message || 'An account with this email address already exists.';
            errorMessageElement.style.display = 'block';
        } else if (data.success === true) {
            errorMessageElement.style.display = 'none';
            localStorage.setItem('name', name);
            localStorage.setItem('subscriptionsTable', JSON.stringify([]));
            window.location.href = 'subscriptions.html';
        } else {
            errorMessageElement.textContent = 'An unexpected response was received. Please try again.';
            errorMessageElement.style.display = 'block';
        }
    }, 1000);
}

function validateEmail(email) {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
}
