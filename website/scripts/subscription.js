document.addEventListener('DOMContentLoaded', () => {
    let vehiclesTable;
    try {
        vehiclesTable = JSON.parse(localStorage.getItem('vehiclesTable')) || [];
    } catch (error) {
        console.error('Error parsing vehiclesTable from localStorage:', error);
        vehiclesTable = [];
    }

    const tableBody = document.querySelector('#vehiclesTable tbody');
    const tableErrorMessage = document.getElementById('tableErrorMessage');
    let selectedRows = [];

    uncheckAllButton.title = "Uncheck all";
    checkAllButton.title = "Check all";
    viewAllButton.title = "View history of all"
    addButton.title = "Add";
    deleteButton.title = "Delete";

    function appendRow(vehicle) {
        const row = document.createElement('tr');
        const firstCell = document.createElement('td');
        firstCell.classList.add('subscription-cell');

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
            uncheckAllButton.disabled = selectedRows.length === 0;
            viewAllButton.disabled = selectedRows.length === 0;
            deleteButton.disabled = selectedRows.length === 0;
        });

        const licensePlateText = document.createElement('span');
        licensePlateText.textContent = vehicle[0] || '';
        checkboxTextWrapper.appendChild(checkbox);
        checkboxTextWrapper.appendChild(licensePlateText);

        const viewButton = document.createElement('button');
        viewButton.title = "View history";

        const viewButtonIcon = document.createElement('img');
        viewButtonIcon.src = 'data/view_history.png';
        viewButtonIcon.alt = 'View';
        viewButtonIcon.classList.add('icon');
        viewButton.appendChild(viewButtonIcon);
        viewButton.classList.add('view-button');

        viewButton.addEventListener('click', (event) => {
            event.stopPropagation();
            const isExpanded = row.classList.contains('expanded');
            if (isExpanded) {
                toggleHistoryRows(row, []);
                row.classList.remove('expanded');
                return;
            }
            const licensePlate = row.querySelector('.checkbox-text-wrapper span').textContent.trim();
            fetch('http://localhost:8080/api/getVehicleHistory', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: new URLSearchParams({ licensePlate })
            })
            .then(response => response.json())
            .then(data => {
                if (data.success && Array.isArray(data.history)) {
                    toggleHistoryRows(row, data.history);
                }
            })
            .catch(error => {
                console.error('Error:', error);
                tableErrorMessage.textContent = 'Error communicating with the server.';
            });
        });

        wrapper.appendChild(checkboxTextWrapper);
        wrapper.appendChild(viewButton);
        firstCell.appendChild(wrapper);
        row.appendChild(firstCell);

        const numColumns = 8;
        for (let i = 1; i < numColumns; i++) {
            const cell = document.createElement('td');
            cell.textContent = vehicle[i] || '';
            if (i === numColumns - 1) {
                cell.style.color = vehicle[i]?.toLowerCase() === 'active' ? 'green' : 'red';
            }
            row.appendChild(cell);
        }
        tableBody.appendChild(row);
    }

    vehiclesTable.forEach(vehicle => appendRow(vehicle));

    uncheckAllButton.addEventListener('click', () => {
        selectedRows.forEach(row => {
            const checkbox = row.querySelector('input[type="checkbox"]');
            if (checkbox) {
                checkbox.checked = false;
                row.classList.remove('selected-row');
            }
        });
        selectedRows = [];
        uncheckAllButton.disabled = true;
        viewAllButton.disabled = true;
        deleteButton.disabled = true;
    });

    let lastAllCheckedState = false;
    checkAllButton.addEventListener('click', () => {
        const checkboxes = tableBody.querySelectorAll('input[type="checkbox"]');
        const allChecked = Array.from(checkboxes).every(checkbox => checkbox.checked);
        if (lastAllCheckedState && allChecked) return;

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

        uncheckAllButton.disabled = selectedRows.length === 0;
        viewAllButton.disabled = selectedRows.length === 0;
        deleteButton.disabled = selectedRows.length === 0;
        lastAllCheckedState = !allChecked;
    });

    viewAllButton.addEventListener('click', () => {
        selectedRows.forEach(row => {
            const licensePlate = row.querySelector('.checkbox-text-wrapper').textContent.trim();
            const urlEncodedData = new URLSearchParams();
            urlEncodedData.append('licensePlate', licensePlate);

            fetch('http://localhost:8080/api/getVehicleHistory', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: urlEncodedData
            })
            .then(response => response.json())
            .then(data => {
                if (data.success && Array.isArray(data.history)) {
                    toggleHistoryRows(row, data.history);
                }
            })
            .catch(error => {
                console.error('Error:', error);
                tableErrorMessage.textContent = 'Error communicating with the server.';
            });
        });

        selectedRows.forEach(row => {
            const checkbox = row.querySelector('input[type="checkbox"]');
            if (checkbox) {
                checkbox.checked = false;
                row.classList.remove('selected-row');
            }
        });

        selectedRows = [];
        uncheckAllButton.disabled = true;
        viewAllButton.disabled = true;
        deleteButton.disabled = true;
    });

    function toggleHistoryRows(row, historyData) {
        const isExpanded = row.classList.toggle('expanded');
        if (isExpanded) {
            const reversedHistoryData = [...historyData].reverse();
            const statusCell = row.querySelector('td:last-child');
            const vehicleStatus = statusCell ? statusCell.textContent.trim() : '';
            if (vehicleStatus === 'Inactive') reversedHistoryData.pop();

            reversedHistoryData.forEach(historyEntry => {
                const newRow = document.createElement('tr');
                newRow.classList.add('history-row', 'fall-in');
                const numColumns = 8;
                for (let i = 0; i < numColumns; i++) {
                    const newCell = document.createElement('td');
                    if (i === 1) newCell.textContent = historyEntry[0] || '';
                    else if (i === 2) newCell.textContent = historyEntry[1] || '';
                    else if (i === 3) newCell.textContent = historyEntry[2] || '';
                    else if (i === 4) newCell.textContent = historyEntry[3] || '';
                    newRow.appendChild(newCell);
                }
                row.parentNode.insertBefore(newRow, row.nextSibling);
            });
        } else {
            let nextSibling = row.nextSibling;
            while (nextSibling && nextSibling.classList.contains('history-row')) {
                nextSibling.remove();
                nextSibling = row.nextSibling;
            }
        }
    }

    addButton.addEventListener('click', () => {
		const checkboxes = document.querySelectorAll('#vehiclesTable input[type="checkbox"]');
		checkboxes.forEach(checkbox => {
			checkbox.checked = false;
			const row = checkbox.closest('tr');
			if (row) {
				row.classList.remove('selected-row');
			}
		});
		
        const newRow = document.createElement('tr');
        const newLicensePlateCell = document.createElement('td');
        newLicensePlateCell.contentEditable = 'true';
        newLicensePlateCell.textContent = '';

        let isSaved = false;

        function saveChanges() {
            if (newLicensePlateCell.textContent.trim() === '') {
                newRow.remove();
            } else {
                const licensePlate = newLicensePlateCell.textContent.trim();
                if (licensePlate) {
                    const name = localStorage.getItem('name');
                    const subscriptionName = localStorage.getItem('subscriptionName');

                    const urlEncodedData = new URLSearchParams();
                    urlEncodedData.append('name', name);
                    urlEncodedData.append('licensePlate', licensePlate);
                    urlEncodedData.append('subscriptionName', subscriptionName);

                    fetch('http://localhost:8080/api/addVehicle', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/x-www-form-urlencoded'
                        },
                        body: urlEncodedData
                    })
                        .then(response => response.json())
                        .then(data => {
                            if (data.success) {
                                appendRow(data.vehiclesTable[0]);
                                localStorage.setItem('vehiclesTable', JSON.stringify([
                                    ...vehiclesTable,
                                    ...data.vehiclesTable
                                ]));
                                newRow.remove();
                            } else {
                                tableErrorMessage.textContent = data.message || 'The vehicle already exists.';
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

        newLicensePlateCell.addEventListener('focusout', () => {
            if (!isSaved) {
                saveChanges();
            }
        });

        newLicensePlateCell.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                e.preventDefault();
                if (!isSaved) {
                    saveChanges();
                }
            }
        });

        newRow.appendChild(newLicensePlateCell);

        const numColumns = 8;
        for (let i = 1; i < numColumns; i++) {
            const emptyCell = document.createElement('td');
            newRow.appendChild(emptyCell);
        }

        tableBody.appendChild(newRow);
        newLicensePlateCell.focus();

        selectedRows = [];
        uncheckAllButton.disabled = true;
        viewAllButton.disabled = true;
        deleteButton.disabled = true;
    });

    deleteButton.addEventListener('click', () => {
        selectedRows.forEach(row => {
            const licensePlate = row.querySelector('.checkbox-text-wrapper').textContent.trim();
            const name = localStorage.getItem('name');
            const subscriptionName = localStorage.getItem('subscriptionName');

            const urlEncodedData = new URLSearchParams();
            urlEncodedData.append('name', name);
            urlEncodedData.append('licensePlate', licensePlate);
            urlEncodedData.append('subscriptionName', subscriptionName);

            fetch('http://localhost:8080/api/deleteVehicle', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                body: urlEncodedData
            })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        let nextSibling = row.nextSibling;
                        while (nextSibling && nextSibling.classList.contains('history-row')) {
                            nextSibling.remove();
                            nextSibling = row.nextSibling;
                        }
                        row.remove();

                        vehiclesTable = vehiclesTable.filter(vehicle => vehicle[0] !== licensePlate);
                        localStorage.setItem('vehiclesTable', JSON.stringify(vehiclesTable));
                    } else {
                        tableErrorMessage.textContent = 'Failed to delete vehicle. Please try again.';
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    tableErrorMessage.textContent = 'An error occurred. Please try again later.';
                });
        });
        selectedRows = [];
        uncheckAllButton.disabled = true;
        viewAllButton.disabled = true;
        deleteButton.disabled = true;
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
