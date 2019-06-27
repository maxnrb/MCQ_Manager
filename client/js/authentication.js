$('#login-send').unbind('click').click(function (event) {
    validateLogin(event);
});

$('#disconnect_btn').unbind('click').click(function () {
    Cookies.set('token', '');
    document.location.href="login.html";
});

function validateLogin(event) {
    let login = $('#username').val();
    let password = $('#password').val();

    if(login.length === 0 || password.length === 0) {
        let text;

        text = '<div class="alert alert-danger" role="alert">';
        text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';

        if(login.length === 0 ) {
            iziToast.show({
                title: 'No username filled in !',
                color: 'red',
                position : "topCenter"
            });

        } else if(password.length === 0) {
            iziToast.show({
                title: 'No password filled in !',
                color: 'red',
                position : "topCenter"
            });

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

        } else if(xhr.status === 401) {
            $("#username").val("");
            $("#password").val("");

            iziToast.show({
                title: 'Incorrect information !',
                color: 'red',
                position : "topCenter"
            });

        } else {
            console.log('HTTP error:' + xhr.status);
        }
    };

    xhr.send();
}