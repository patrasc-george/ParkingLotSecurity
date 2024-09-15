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
    const addButton = document.getElementById('addButton');
    const deleteButton = document.getElementById('deleteButton');
    
    let selectedRow = null;

    function appendRow(vehicle) {
        const row = document.createElement('tr');
        let clickTimeout;

        const numColumns = 8;
        for (let i = 0; i < numColumns; i++) {
            const cell = document.createElement('td');
            cell.textContent = vehicle[i] || '';
            if (i === numColumns - 1) {
                cell.style.color = vehicle[i]?.toLowerCase() === 'active' ? 'green' : 'red';
            }
            row.appendChild(cell);
        }

        function toggleHistoryRows(row, historyData) {
            const isExpanded = row.classList.toggle('expanded');
            if (isExpanded) {
                const reversedHistoryData = [...historyData].reverse();

                if (vehicle[7]?.toLowerCase() !== 'active') {
                    reversedHistoryData.pop();
                }

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

        row.addEventListener('dblclick', (event) => {
            event.stopPropagation();
            clearTimeout(clickTimeout);

            if (row.classList.contains('expanded')) {
                toggleHistoryRows(row, []);
                return;
            }

            const licensePlate = row.cells[0].textContent;

            fetch('http://localhost:8080/api/getVehicleHistory', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
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

    vehiclesTable.forEach(vehicle => appendRow(vehicle));

    addButton.addEventListener('click', () => {
        const newRow = document.createElement('tr');
        
        const newLicensePlateCell = document.createElement('td');
        newLicensePlateCell.contentEditable = 'true';
        newLicensePlateCell.textContent = '';
        
        let isSaved = false; // Flag pentru a preveni salvarea de două ori
        
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
    });    

    deleteButton.addEventListener('click', () => {
        if (selectedRow) {
            const licensePlate = selectedRow.cells[0].textContent;
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
                    let nextSibling = selectedRow.nextSibling;
                    while (nextSibling && nextSibling.classList.contains('history-row')) {
                        nextSibling.remove();
                        nextSibling = selectedRow.nextSibling;
                    }
                    selectedRow.remove();
    
                    vehiclesTable = vehiclesTable.filter(vehicle => vehicle[0] !== licensePlate);
                    localStorage.setItem('vehiclesTable', JSON.stringify(vehiclesTable));
    
                    selectedRow = null;
                    deleteButton.disabled = true;
                } else {
                    tableErrorMessage.textContent = 'Failed to delete vehicle. Please try again.';
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
        localStorage.removeItem('vehiclesTable');
        window.location.href = 'login.html';
    });
});
