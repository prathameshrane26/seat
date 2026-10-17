const canvas = document.getElementById('venue-canvas');
const ctx = canvas ? canvas.getContext('2d') : null;

function resizeCanvas() {
    if (!canvas) return;
    canvas.width = canvas.parentElement.clientWidth;
    canvas.height = canvas.parentElement.clientHeight || 380;
    drawVenueLayout();
}

function drawVenueLayout() {
    if (!ctx || !canvas) return;
    
    // Clear canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Background tint
    ctx.fillStyle = '#faf8f5';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw Mock Tables (Circles with seats)
    const tables = [
        { x: 120, y: 120, name: 'Table 1', seats: 6 },
        { x: 320, y: 120, name: 'Table 2', seats: 6 },
        { x: 220, y: 280, name: 'Table 3', seats: 8 }
    ];

    tables.forEach(t => {
        // Table Circle
        ctx.beginPath();
        ctx.arc(t.x, t.y, 40, 0, Math.PI * 2);
        ctx.fillStyle = '#d4b886';
        ctx.fill();
        ctx.lineWidth = 2;
        ctx.strokeStyle = '#8c6d46';
        ctx.stroke();

        // Table Label
        ctx.fillStyle = '#2c221e';
        ctx.font = 'bold 12px Inter, sans-serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(t.name, t.x, t.y);

        // Seats around table
        for (let i = 0; i < t.seats; i++) {
            const angle = (i * 2 * Math.PI) / t.seats;
            const sx = t.x + Math.cos(angle) * 60;
            const sy = t.y + Math.sin(angle) * 60;

            ctx.beginPath();
            ctx.arc(sx, sy, 10, 0, Math.PI * 2);
            ctx.fillStyle = '#ffffff';
            ctx.fill();
            ctx.strokeStyle = '#b89762';
            ctx.stroke();
        }
    });
}

window.addEventListener('resize', resizeCanvas);
document.addEventListener('DOMContentLoaded', resizeCanvas);