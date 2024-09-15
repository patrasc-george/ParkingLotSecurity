document.addEventListener('DOMContentLoaded', () => {
    let subscriptionsTable;
    try {
        subscriptionsTable = JSON.parse(localStorage.getItem('subscriptionsTable')) || [];
    } catch (error) {
        console.error('Error parsing subscriptionsTable from localStorage:', error);
        subscriptionsTable = [];
    }

    const tableBody = document.querySelector('#subscriptionsTable');
    const tableErrorMessage = document.getElementById('tableErrorMessage');
    const addButton = document.getElementById('addButton');
    const deleteButton = document.getElementById('deleteButton');
    
    let selectedRow = null;

    function appendRow(subscription) {
        const row = document.createElement('tr');
        let clickTimeout;

        const cell = document.createElement('td');
        cell.textContent = subscription;
        row.appendChild(cell);

        row.addEventListener('dblclick', (event) => {
            event.stopPropagation();
            clearTimeout(clickTimeout);
        
            const subscriptionName = row.cells[0].textContent;
            const name = localStorage.getItem('name');
        
            const urlEncodedData = new URLSearchParams();
            urlEncodedData.append('name', name);
            urlEncodedData.append('subscriptionName', subscriptionName);
        
            fetch('http://localhost:8080/api/getSubscriptionVehicles', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                body: urlEncodedData
            })
            .then(response => response.json())
            .then(data => {
                if (data.success && Array.isArray(data.vehiclesTable)) {
                    localStorage.setItem('vehiclesTable', JSON.stringify(data.vehiclesTable));
                    localStorage.setItem('subscriptionName', subscriptionName);
                    window.location.href = 'subscription.html';
                } else {
                    tableErrorMessage.textContent = 'Error fetching vehicles for subscription.';
                }
            })
            .catch(error => {
                console.error('Error:', error);
                tableErrorMessage.textContent = 'Error communicating with the server.';
            });
        });        

        row.addEventListener('click', () => {
            clickTimeout = setTimeout(() => {
                if (row === selectedRow) {
                    row.classList.remove('selected-row');
                    selectedRow = null;
                    deleteButton.disabled = true;
                } else {
                    if (selectedRow) {
                        selectedRow.classList.remove('selected-row');
                    }
                    row.classList.add('selected-row');
                    selectedRow = row;
                    deleteButton.disabled = false;
                }
            }, 200);
        });

        tableBody.appendChild(row);
    }

    subscriptionsTable.forEach(subscription => appendRow(subscription));

    addButton.addEventListener('click', () => {
        const newRow = document.createElement('tr');
        
        const newSubscriptionCell = document.createElement('td');
        newSubscriptionCell.contentEditable = 'true';
        newSubscriptionCell.textContent = '';
        
        let isSaved = false; // Flag pentru a preveni salvarea de două ori
        
        function saveChanges() {
            if (newSubscriptionCell.textContent.trim() === '') {
                newRow.remove();
            } else {
                const subscriptionName = newSubscriptionCell.textContent.trim();
                if (subscriptionName) {
                    const name = localStorage.getItem('name');
                    
                    const urlEncodedData = new URLSearchParams();
                    urlEncodedData.append('name', name);
                    urlEncodedData.append('subscriptionName', subscriptionName);
                    
                    fetch('http://localhost:8080/api/addSubscription', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/x-www-form-urlencoded'
                        },
                        body: urlEncodedData
                    })
                    .then(response => response.json())
                    .then(data => {
                        if (data.success) {
                            appendRow(subscriptionName);
                            subscriptionsTable.push(subscriptionName);
                            localStorage.setItem('subscriptionsTable', JSON.stringify(subscriptionsTable));
                            newRow.remove();
                        } else {
                            tableErrorMessage.textContent = 'The subscription already exists.';
                            newRow.remove();
                        }
                    })
                    .catch(error => {
                        console.error('Error:', error);
                        tableErrorMessage.textContent = 'An error occurred. Please try again later.';
                        newRow.remove();
                    });
                } else {
                    newRow.remove();
                }
                isSaved = true;
            }
        }
    
        newSubscriptionCell.addEventListener('focusout', () => {
            if (!isSaved) {
                saveChanges();
            }
        });
    
        newSubscriptionCell.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                e.preventDefault();
                if (!isSaved) {
                    saveChanges();
                }
            }
        });
    
        newRow.appendChild(newSubscriptionCell);
        tableBody.appendChild(newRow);
        newSubscriptionCell.focus();
    });    
    
    deleteButton.addEventListener('click', () => {
        if (selectedRow) {
            const subscriptionName = selectedRow.textContent;
            const name = localStorage.getItem('name');
    
            const urlEncodedData = new URLSearchParams();
            urlEncodedData.append('name', name);
            urlEncodedData.append('subscriptionName', subscriptionName);
    
            fetch('http://localhost:8080/api/deleteSubscription', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                body: urlEncodedData
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    selectedRow.remove();
                    subscriptionsTable = subscriptionsTable.filter(sub => sub !== subscriptionName);
                    localStorage.setItem('subscriptionsTable', JSON.stringify(subscriptionsTable));
    
                    selectedRow = null;
                    deleteButton.disabled = true;
                } else {
                    tableErrorMessage.textContent = 'Failed to delete subscription. Please try again.';
                }
            })
            .catch(error => {
                console.error('Error:', error);
                tableErrorMessage.textContent = 'An error occurred. Please try again later.';
            });
        }
    });    

    const logoutButton = document.getElementById('logoutButton');
    logoutButton.addEventListener('click', () => {
        localStorage.removeItem('subscriptionsTable');
        window.location.href = 'login.html';
    });
});
