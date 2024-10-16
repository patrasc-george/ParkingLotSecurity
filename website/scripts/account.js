document.addEventListener('DOMContentLoaded', () => {
    nameDropdownButton.title = "Change";
    passwordDropdownButton.title = "Change";

    nameDropdownButton.addEventListener('click', () => {
        nameDropdownContent.classList.toggle('active');
        passwordDropdownContent.classList.remove('active');
    });

    passwordDropdownButton.addEventListener('click', () => {
        passwordDropdownContent.classList.toggle('active');
        nameDropdownContent.classList.remove('active');
    });

    document.getElementById('nameForm').addEventListener('submit', function(event) {
        event.preventDefault();
    
        const currentPasswordName = document.getElementById('currentPasswordName').value;
        const newName = document.getElementById('newName').value;
        const errorMessageElement = document.getElementById('errorMessage');
        const successMessageElement = document.getElementById('successMessage');
    
        errorMessageElement.textContent = '';
        successMessageElement.textContent = '';
    
        const name = localStorage.getItem('name');
        if (!validateName(newName)) {
            errorMessageElement.textContent = "Please enter a valid email address.";
            return;
        }

        if (!currentPasswordName || !newName) {
            errorMessageElement.textContent = "Please enter both current password and new email.";
            return;
        }
    
        const urlEncodedData = new URLSearchParams();
        urlEncodedData.append('name', name);
        urlEncodedData.append('currentPassword', currentPasswordName);
        urlEncodedData.append('newName', newName);
    
        fetch('http://localhost:8080/api/updateName', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: urlEncodedData
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                localStorage.setItem('name', newName);
                successMessageElement.textContent = 'Password updated successfully.';
            } else if (data.message === 'Email already exists.') {
                    errorMessageElement.textContent = 'The email address already exists.';
                } else if (data.message === 'The current password is incorrect.') {
                    errorMessageElement.textContent = 'The current password is incorrect.';
                } else {
                    errorMessageElement.textContent = 'Failed to update email.';
                }            
        })
        .catch(error => {
            console.error('Error:', error);
            errorMessageElement.textContent = 'An error occurred. Please try again later.';
        });
    });

    document.getElementById('passwordForm').addEventListener('submit', function(event) {
        event.preventDefault();
    
        const currentPassword = document.getElementById('currentPassword').value;
        const newPassword = document.getElementById('newPassword').value;
        const errorMessageElement = document.getElementById('errorMessage');
        const successMessageElement = document.getElementById('successMessage');
    
        errorMessageElement.textContent = '';
        successMessageElement.textContent = '';
    
        if (!currentPassword || !newPassword) {
            errorMessageElement.textContent = "Please enter both current password and new password.";
            return;
        }
    
        const urlEncodedData = new URLSearchParams();
        const name = localStorage.getItem('name');
        urlEncodedData.append('name', name);
        urlEncodedData.append('currentPassword', currentPassword);
        urlEncodedData.append('newPassword', newPassword);
    
        fetch('http://localhost:8080/api/updatePassword', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded'
            },
            body: urlEncodedData
        })
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                successMessageElement.textContent = 'Password updated successfully.';
            } else {
                errorMessageElement.textContent = 'The current password is incorrect.';
            }
        })
        .catch(error => {
            console.error('Error:', error);
            errorMessageElement.textContent = 'An error occurred. Please try again later.';
        });
    });
    
    accountIcon.addEventListener('click', function(event) {
        const dropdown = document.getElementById('accountDropdown');
        dropdown.classList.toggle('hidden');
        event.stopPropagation();
    });

    document.addEventListener('click', function(event) {
        const dropdown = document.getElementById('accountDropdown');
        if (!dropdown.classList.contains('hidden') && event.target !== accountIcon) {
            dropdown.classList.add('hidden');
        }
    });

    settingsOption.addEventListener('click', function() {
        window.location.href = 'account.html';
    });

    subscriptionsOption.addEventListener('click', function() {
        window.location.href = 'subscriptions.html';
    });

    logoutOption.addEventListener('click', function() {
        window.location.href = 'login.html';
    });

    function validateName(name) {
        const nameRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        return nameRegex.test(name);
    }    
});
