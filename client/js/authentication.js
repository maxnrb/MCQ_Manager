$('#login-send').unbind('click').click(function (event) {
    validateLogin(event);
});

$('#disconnect').unbind('click').click(function () {
    Cookies.set('token', '');
    document.getElementById('login-form').style.display = 'block';
    document.getElementById('disconnect_div').style.display = 'none';

    let text;

    text = '<div class="alert alert-success" role="alert">';
    text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';
    text += '<strong> Déconnecté avec succèes</strong>';
    text += '</div>';
    $('#information').html(text);
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
    xhr.open('GET', 'php/request.php/authenticate');
    xhr.setRequestHeader('Authorization', 'Basic ' + btoa(login + ':' + password));

    xhr.onload = function () {
        if (xhr.status === 200) {
            Cookies.set('token', xhr.responseText);

            document.getElementById('login-form').style.display = 'none';
            document.getElementById('disconnect_div').style.display = 'block';

            let text;

            text = '<div class="alert alert-success" role="alert">';
            text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';
            text += '<strong> Connexion Réussie</strong>';
            text += '</div>';
            $('#information').html(text);

            ajaxRequest('GET', "php/request.php/photos/", displayPicture);

        } else {
            console.log('HTTP error:' + xhr.status);
        }
    };

    xhr.send();
}

function displayPicture(ajaxResponse) {
    console.log(ajaxResponse);

    let text;

    text = '<div class="alert alert-success" role="alert">';
    text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';
    text += '<strong> Connecté</strong>';
    text += '</div>';
    $('#information').html(text);

    document.getElementById('disconnect_div').style.display = 'block';
}

ajaxRequest('POST', "php/request.php/photos/", displayPicture);