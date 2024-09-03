const serverUrl = 'http://localhost:8080/api/endpoint';

const qrCodeButton = document.getElementById('qrCode');
const qrInput = document.getElementById('qrInput');
const fileNameElement = document.getElementById('fileName');
const errorMessageElement = document.getElementById('errorMessage');
const successMessageElement = document.getElementById('successMessage'); // Corectare ID-ului
const licensePlateInput = document.getElementById('licensePlate');
let selectedFile = null;

qrCodeButton.addEventListener('click', function () {
    qrInput.click();
});

qrInput.addEventListener('change', function (event) {
    selectedFile = event.target.files[0];
    if (selectedFile) {
        licensePlateInput.value = '';
        fileNameElement.style.display = 'block';
        fileNameElement.textContent = `Uploaded file: ${selectedFile.name}`;
        errorMessageElement.textContent = '';
    }
});

document.querySelector('.payButton').addEventListener('click', function () {
    const licensePlate = licensePlateInput.value;
    errorMessageElement.textContent = '';

    if (!licensePlate && !selectedFile) {
        errorMessageElement.textContent = 'Please enter a license plate number or upload a QR code.';
        return;
    }

    if (licensePlate) {
        selectedFile = null;
        fileNameElement.style.display = 'none';
    }

    if (licensePlate) {
        const urlEncodedData = new URLSearchParams();
        urlEncodedData.append('licensePlate', licensePlate);

        fetch(serverUrl, {
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
    } else if (selectedFile) {
        const formData = new FormData();
        formData.append('qrCodeImage', selectedFile, selectedFile.name);

        fetch(serverUrl, {
            method: 'POST',
            body: formData
        })
            .then(response => response.json())
            .then(data => handleServerResponse(data))
            .catch(error => {
                console.error('Error:', error);
                errorMessageElement.textContent = 'An error occurred. Please try again later.';
            });
    }
});

function handleServerResponse(data) {
    console.log('Server response:', data);

    errorMessageElement.textContent = '';
    successMessageElement.textContent = '';

    setTimeout(() => {
        if (data.success === false) {
            errorMessageElement.textContent = 'The vehicle was not found. Please upload the QR code again.';
            successMessageElement.style.display = 'none';
            errorMessageElement.style.display = 'block';
        } else if (data.success === true) {
            const licensePlate = data.licensePlate || licensePlateInput.value;
            const dateTime = data.dateTime || new Date().toLocaleString();

            successMessageElement.textContent = `The vehicle with license plate number ${licensePlate} was found. The parking fee has been paid. The vehicle entered the parking lot on ${dateTime}.`;
            errorMessageElement.style.display = 'none';
            successMessageElement.style.display = 'block';
        } else {
            errorMessageElement.textContent = 'An unexpected response was received. Please try again.';
            successMessageElement.style.display = 'none';
            errorMessageElement.style.display = 'block';
        }
    }, 1000);
}

document.addEventListener('DOMContentLoaded', function () {
    const subscriptionsButton = document.getElementById('subscriptionsButton');
    if (subscriptionsButton) {
        subscriptionsButton.addEventListener('click', function () {
            window.location.href = 'login.html';
        });
    }
});
