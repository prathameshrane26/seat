// Handles guest relationship constraints (must-sit-together / conflicts)
const RelationshipMatrix = {
    rules: [],

    addRule(guestAId, guestBId, relationType) {
        this.rules.push({ guestAId, guestBId, relationType }); // 'together' or 'apart'
        console.log(`Added relationship rule: Guest #${guestAId} & Guest #${guestBId} -> ${relationType}`);
    },

    getRules() {
        return this.rules;
    },

    clear() {
        this.rules = [];
    }
};

console.log("Relationship Matrix module loaded.");