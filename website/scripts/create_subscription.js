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
            errorMessageElement.textContent = data.message || 'Subscription creation failed. Please try again.';
            errorMessageElement.style.display = 'block';
        } else if (data.success === true) {
            errorMessageElement.style.display = 'none';
			window.location.href = 'login.html';
        } else {
            errorMessageElement.textContent = 'An unexpected response was received. Please try again.';
            errorMessageElement.style.display = 'block';
        }
    }, 1000);
}
