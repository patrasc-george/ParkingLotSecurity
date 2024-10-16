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
    let selectedRows = [];

    uncheckAllButton.title = "Uncheck all";
    checkAllButton.title = "Check all";
    addButton.title = "Add";
    deleteButton.title = "Delete";

    function appendRow(subscription) {
        const row = document.createElement('tr');
        const cell = document.createElement('td');
        cell.classList.add('subscription-cell');

        const wrapper = document.createElement('div');
        wrapper.classList.add('cell-wrapper');

        const checkboxTextWrapper = document.createElement('div');
        checkboxTextWrapper.classList.add('checkbox-text-wrapper');

        const checkbox = document.createElement('input');
        checkbox.type = 'checkbox';

        checkbox.addEventListener('change', () => {
            if (checkbox.checked) {
                row.classList.add('selected-row');
                selectedRows.push(row);
            } else {
                row.classList.remove('selected-row');
                selectedRows = selectedRows.filter(selectedRow => selectedRow !== row);
            }
            deleteButton.disabled = selectedRows.length === 0;
            uncheckAllButton.disabled = selectedRows.length === 0;
        });

        const subscriptionText = document.createElement('span');
        subscriptionText.textContent = subscription;

        checkboxTextWrapper.appendChild(checkbox);
        checkboxTextWrapper.appendChild(subscriptionText);

        const viewButton = document.createElement('button');
        viewButton.title = "View";
        
        const viewButtonIcon = document.createElement('img');
        viewButtonIcon.src = 'data/select.png';
        viewButtonIcon.alt = 'View';
        viewButtonIcon.classList.add('icon');

        viewButton.appendChild(viewButtonIcon);
        viewButton.classList.add('view-button');

        viewButton.addEventListener('click', (event) => {
            event.stopPropagation();
            const subscriptionName = subscriptionText.textContent;
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

        wrapper.appendChild(checkboxTextWrapper);
        wrapper.appendChild(viewButton);
        cell.appendChild(wrapper);
        row.appendChild(cell);
        tableBody.appendChild(row);
    }

    subscriptionsTable.forEach(subscription => appendRow(subscription));

    uncheckAllButton.addEventListener('click', () => {
        selectedRows.forEach(row => {
            const checkbox = row.querySelector('input[type="checkbox"]');
            if (checkbox) {
                checkbox.checked = false;
                row.classList.remove('selected-row');
            }
        });
        selectedRows = [];
        deleteButton.disabled = true;
        uncheckAllButton.disabled = true;
    });

    checkAllButton.addEventListener('click', () => {
        const checkboxes = tableBody.querySelectorAll('input[type="checkbox"]');
        const allChecked = Array.from(checkboxes).every(checkbox => checkbox.checked);

        checkboxes.forEach(checkbox => {
            checkbox.checked = !allChecked;
            const row = checkbox.closest('tr');
            if (checkbox.checked) {
                row.classList.add('selected-row');
                if (!selectedRows.includes(row)) {
                    selectedRows.push(row);
                }
            } else {
                row.classList.remove('selected-row');
                selectedRows = selectedRows.filter(selectedRow => selectedRow !== row);
            }
        });

        deleteButton.disabled = selectedRows.length === 0;
        uncheckAllButton.disabled = selectedRows.length === 0;
    });

    addButton.addEventListener('click', () => {
        const checkboxes = document.querySelectorAll('#subscriptionsTable input[type="checkbox"]');
        checkboxes.forEach(checkbox => {
            checkbox.checked = false;
            const row = checkbox.closest('tr');
            if (row) {
                row.classList.remove('selected-row');
            }
        });

        const newRow = document.createElement('tr');
        const newSubscriptionCell = document.createElement('td');
        newSubscriptionCell.contentEditable = 'true';
        newSubscriptionCell.textContent = '';
        let isSaved = false;

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

        deleteButton.disabled = true;
        uncheckAllButton.disabled = true;
    });

    deleteButton.addEventListener('click', () => {
        selectedRows.forEach(row => {
            const subscriptionName = row.querySelector('span').textContent.trim();
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
                    row.remove();
                    subscriptionsTable = subscriptionsTable.filter(sub => sub !== subscriptionName);
                    localStorage.setItem('subscriptionsTable', JSON.stringify(subscriptionsTable));
                } else {
                    tableErrorMessage.textContent = 'Failed to delete subscription. Please try again.';
                }
            })
            .catch(error => {
                console.error('Error:', error);
                tableErrorMessage.textContent = 'An error occurred. Please try again later.';
            });
        });

        selectedRows = [];
        deleteButton.disabled = true;
        uncheckAllButton.disabled = true;
    });

    accountIcon.addEventListener('click', function(event) {
        const dropdown = document.getElementById('accountDropdown');
        dropdown.style.display = dropdown.style.display === 'none' ? 'block' : 'none';
        event.stopPropagation();
    });
    
    document.addEventListener('click', function(event) {
        const dropdown = document.getElementById('accountDropdown');
        if (event.target !== dropdown && event.target !== document.getElementById('accountIcon')) {
            dropdown.style.display = 'none';
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
});
