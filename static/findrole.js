function find(userid, callback) {
    if (!userid) return;
    var baseUrl = window.location.origin;
    fetch(baseUrl + "/findrole", {
        method: "POST",
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: userid })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success === 1) {
            callback(null, data.role);
        } else {
            callback(data.message);
        }
    })
    .catch(err => callback("网络错误"));
}

