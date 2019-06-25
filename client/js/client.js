document.getElementById("chooseGroup").addEventListener("click", function(){
    let option = document.querySelector("#group_selector > option:checked");
    console.log("Group selected");

    ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
        showStudents(ajaxResponse);
    }, 'group_id=' + option.id );

    ajaxRequest('GET', addressIP + "/tests", function (ajaxResponse) {
        showTests(ajaxResponse);
    }, 'group_id=' + option.id );
});


function showTests(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);

    $('#test_tbody').html('');

    let option = document.querySelector("#group_selector > option:checked");

    for(let i = 0; i < data.length; i++) {
        $('#test_tbody').append(
            '<tr>' +
            '<td><input id=test_date' + data[i].id + ' class="datepicker form-control" type="text" value="' + data[i].date + '"/></td>' +
            '<td><input id=test_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].name + '"/></td>' +
            '<td><button id=tmod' + data[i].id + ' class="btn btn-success">Edit</button></td>' +
            '<td><button id=tdel' + data[i].id + ' class="btn btn-danger">Delete</button></td>' +
            '</tr>'
        );

        $('#tmod' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);
            console.log("click");

            ajaxRequest('PUT', addressIP + "/test", function () {
            }, 'test_id=' + id + "&date=" + $('#test_date' + id).val() + "&test_name=" + $('#test_name' + id).val()
            );
        });


        $('#tdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/test", function () {
                ajaxRequest('GET', addressIP + "/tests", function (ajaxResponse) {
                    showTests(ajaxResponse);
                }, 'group_id=' + option.id );
            }, 'test_id=' + id );
        });
    }

    $('.datepicker').datepicker({
        uiLibrary: 'bootstrap4'
    });

}

function showLoginForm() {

}

function showGroupForm() {
    hidePageElement();
    document.getElementById('group_form').style.display = 'block';
}

function showStudentForm() {
    hidePageElement();
    document.getElementById('student_form').style.display = 'block';
}

function showTestForm() {
    hidePageElement();
    document.getElementById('test_form').style.display = 'block';
}

function showUserForm() {

}

function showGroupSelector() {
    hidePageElement();
    document.getElementById('group_selectorDiv').style.display = 'block';
}

function showStudentSelector() {
    document.getElementById('student_selectorDiv').style.display = 'block';
}

function hidePageElement() {
    $('.page-element').css("display", "none");
}

function showManageStudent() {

}

function showManageGroup() {
    hidePageElement();
    $('#group_manageDiv').css("display", "block");
}

function showManageStudent() {
    hidePageElement();
    $('#group_selectorDiv').css("display", "block");
    $('#student_manageDiv').css("display", "block");
}

document.getElementById("home_btn").addEventListener("click",function (event) { showGroupSelector(); });
document.getElementById("group_formBtn").addEventListener("click",function (event) { showGroupForm(); });
document.getElementById("student_formBtn").addEventListener("click",function (event) { showStudentForm(); });
document.getElementById("test_formBtn").addEventListener("click",function (event) { showTestForm(); });
document.getElementById("group_manageBtn").addEventListener("click",function (event) {
    ajaxRequest('GET', addressIP + "/groups", showManageGroup);
});

document.getElementById("student_manageBtn").addEventListener("click",function (event) {
    showManageStudent();
});

ajaxRequest('GET', addressIP + "/groups", showGroups);

ajaxRequest('GET', addressIP + "/image", function (response) {
    //$("body").html("<img src='"+response+"'/>");
});
