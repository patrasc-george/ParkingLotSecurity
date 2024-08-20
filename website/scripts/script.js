const serverUrl = 'http://localhost:8080/api/endpoint';

const qrCodeButton = document.getElementById('qrCode');
const qrInput = document.getElementById('qrInput');
const fileNameElement = document.getElementById('fileName');
const errorMessageElement = document.getElementById('errorMessage');
const successMessageElement = document.getElementById('succesMessage');
const licensePlateInput = document.getElementById('licensePlate');
let selectedFile = null;

qrCodeButton.addEventListener('click', function() {
    qrInput.click();
});

qrInput.addEventListener('change', function(event) {
    selectedFile = event.target.files[0];
    if (selectedFile) {
        licensePlateInput.value = '';

        fileNameElement.style.display = 'block';
        fileNameElement.textContent = `Uploaded file: ${selectedFile.name}`;
        errorMessageElement.textContent = '';
    }
});

document.querySelector('.payButton').addEventListener('click', function() {
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
        .then(response => response.text())
        .then(data => {
            handleServerResponse(data);
        })
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
        .then(response => response.text())
        .then(data => {
            handleServerResponse(data);
        })
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
        if (licensePlateInput.value && data.toLowerCase() === 'false') {
            errorMessageElement.textContent = 'The vehicle was not found. Please upload the QR code.';
            successMessageElement.style.display = 'none';
            errorMessageElement.style.display = 'block';
        } else if (selectedFile && data.toLowerCase() === 'false') {
            errorMessageElement.textContent = 'The vehicle was not found. Please upload the QR code again.';
            successMessageElement.style.display = 'none';
            errorMessageElement.style.display = 'block';
        } else if (data.toLowerCase() === 'true') {
            successMessageElement.textContent = 'The vehicle was found. The parking fee has been paid.';
            errorMessageElement.style.display = 'none';
            successMessageElement.style.display = 'block';
        } else {
            errorMessageElement.textContent = 'An unexpected response was received. Please try again.';
            successMessageElement.style.display = 'none';
            errorMessageElement.style.display = 'block';
        }
    }, 1000);
}
