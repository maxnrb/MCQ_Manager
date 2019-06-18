'use strict';

function ajaxRequest(type, request, callback, data = null) {
  let xhr;

  xhr = new XMLHttpRequest();
  if (type === 'GET' && data !== null) {
    request += '?' + data;
  }
  xhr.open(type, request, true);
  xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  xhr.setRequestHeader('Authorization', 'Bearer ' + Cookies.get('token'));

  // Add the onload function.
  xhr.onload = function () {
    switch (xhr.status)
    {
      case 200:
      case 201:
        callback(xhr.responseText);
        break;

      case 401 :
        document.getElementById('connexion-form').style.display = 'block';
        break;

      case 403 :
        document.getElementById('connexion-form').style.display = 'block';
        break;

      default:
        httpErrors(xhr.status);
    }
  };

  xhr.send(data);
}

function httpErrors(errorNumber) {
  let text;

  text = '<div class="alert alert-danger" role="alert">';
  text += '<span class="glyphicon glyphicon-exclamation-sign"></span>';

  switch (errorNumber) {
    case 400:
      // Bad request.
      text += '<strong> Requête incorrecte</strong>';
      break;

    case 401:
      // Unauthorized.
      text += '<strong> Authentifiez vous</strong>';
      break;

    case 403:
      // Forbidden.
      text += '<strong> Accès refusé</strong>';
      break;

    case 404:
      // Ressource not found.
      text += '<strong> Page non trouvée</strong>';
      break;

    case 500:
      // Internal server error.
      text += '<strong> Erreur interne du serveur</strong>';
      break;

    case 503:
      // Service unavailable.
      text += '<strong> Service indisponible</strong>';
      break;

    default:
      text += '<strong> HTTP erreur ' + errorNumber + '</strong>';
      break;
  }
  text += '</div>';
  $('#errors').html(text);
}
