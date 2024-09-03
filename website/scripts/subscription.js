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

    vehiclesTable.forEach(vehicle => {
        const row = document.createElement('tr');

        if (Array.isArray(vehicle)) {
            vehicle.forEach((cellData, index) => {
                const cell = document.createElement('td');
                cell.textContent = cellData;

                if (index === vehicle.length - 1) {
                    if (cellData.toLowerCase() === 'active') {
                        cell.style.color = 'green';
                    } else if (cellData.toLowerCase() === 'inactive') {
                        cell.style.color = 'red';
                    }
                }

                row.appendChild(cell);
            });

            row.addEventListener('click', () => {
                if (row.classList.contains('selected-row')) {
                    row.classList.remove('selected-row');
                } else {
                    const previouslySelected = tableBody.querySelector('.selected-row');
                    if (previouslySelected) {
                        previouslySelected.classList.remove('selected-row');
                    }
                    row.classList.add('selected-row');
                }
            });

            tableBody.appendChild(row);
        } else {
            console.warn('Invalid vehicle data format:', vehicle);
        }
    });
});
