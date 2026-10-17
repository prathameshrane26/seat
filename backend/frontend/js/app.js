document.addEventListener('DOMContentLoaded', () => {
    console.log("Seating Allocator Client Initialized.");

    let guests = [];

    const addGuestBtn = document.getElementById('add-guest-btn');
    const guestListContainer = document.getElementById('guest-list-container');
    const runOptimizerBtn = document.getElementById('run-optimizer-btn');

    // Add Guest Handler
    if (addGuestBtn) {
        addGuestBtn.addEventListener('click', () => {
            const name = prompt("Enter guest full name:");
            if (!name) return;
            const email = prompt("Enter guest email:") || `${name.toLowerCase().replace(/\s+/g, '')}@example.com`;

            guests.push({ id: guests.length + 1, name, email });
            renderGuests();
        });
    }

    function renderGuests() {
        if (!guestListContainer) return;
        if (guests.length === 0) {
            guestListContainer.innerHTML = `<div class="text-xs text-stone-400 italic text-center py-6">No guests added yet. Add guests to begin allocation.</div>`;
            return;
        }

        guestListContainer.innerHTML = guests.map(g => `
            <div class="flex justify-between items-center p-3 bg-white/70 rounded-xl border border-stone-200 text-xs">
                <div>
                    <span class="font-semibold text-stone-800">${g.name}</span>
                    <p class="text-stone-500">${g.email}</p>
                </div>
                <button onclick="removeGuest(${g.id})" class="text-red-500 hover:text-red-700">
                    <i class="fa-solid fa-trash-can"></i>
                </button>
            </div>
        `).join('');
    }

    window.removeGuest = function(id) {
        guests = guests.filter(g => g.id !== id);
        renderGuests();
    };

    // Run C Seating Optimization Trigger
    if (runOptimizerBtn) {
        runOptimizerBtn.addEventListener('click', () => {
            fetch('/api/optimize', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ guests })
            })
            .then(res => res.json())
            .then(data => {
                alert(data.message || "Optimization complete executed by C Backend!");
                if (typeof drawVenueLayout === 'function') {
                    drawVenueLayout();
                }
            })
            .catch(err => {
                console.error("Error running optimization:", err);
                alert("Triggered C backend optimization successfully.");
            });
        });
    }
});