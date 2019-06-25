$('#login-send').unbind('click').click(function (event) {
    validateLogin(event);
});

$('#disconnect_btn').unbind('click').click(function () {
    Cookies.set('token', '');
    document.location.href="login.html";
    // TODO Msg
});

function validateLogin(event) {
    let login = $('#username').val();
    let password = $('#password').val();

    if(login.length === 0 || password.length === 0) {
        let text;

        text = '<div class="alert alert-danger" role="alert">';
        text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';

        if(login.length === 0 ) {
            text += '<strong> Login non renseigné</strong>';
        } else if(password.length === 0) {
            text += '<strong> Mot de passe non renseigné</strong>';
        }

        text += '</div>';
        $('#information').html(text);

        return;
    }

    let xhr = new XMLHttpRequest();
    xhr.open('GET', addressIP + "/authenticate");
    xhr.setRequestHeader('Authorization', 'Basic ' + btoa(login + ':' + password));

    xhr.onload = function () {
        if (xhr.status === 200) {
            Cookies.set('token', xhr.responseText);
            document.location.href="index.html";
            // TODO Msg
        } else {
            // TODO Msg
            console.log('HTTP error:' + xhr.status);
        }
    };

    xhr.send();
}