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


document.getElementById("group_send").addEventListener("click",function (event) {
    console.log("Add group");

    ajaxRequest('POST', addressIP + "/group", function () {
        console.log("Group added");
    }, 'group_name=' + $('#groupName').val()
    );
});


document.getElementById("student_send").addEventListener("click",function (event) {
    console.log("Add student");

    let option = document.querySelector("#student_group > option:checked");
    console.log(option.id);

    ajaxRequest('POST', addressIP + "/student", function () {
        console.log("Student added");
    }, 'group_id=' + option.id + "&first_name=" + $('#studentFirstName').val() + "&last_name=" + $('#studentLastName').val()
    );
});


document.getElementById("admin_send").addEventListener("click",function (event) {
    console.log("Add admin");

    ajaxRequest('POST', addressIP + "/user", function () {
        console.log("Admin added");
    }, 'login=' + $('#adminName').val() + "&password=" + $('#adminPassword').val()
    );
});


document.getElementById("test_send").addEventListener("click",function (event) {
    console.log("Add test");

    let option = document.querySelector("#test_group > option:checked");
    console.log(option.id);

    ajaxRequest('POST', addressIP + "/test", function () {
        console.log("Test added");
    }, 'group_id=' + option.id + "&date=" + $('#datepicker').val() + "&name=" + $('#testName').val()
    );
});


function showGroups(ajaxResponse) {
    // TODO : verifier si pas de groupe

    console.log(ajaxResponse);
    let data = JSON.parse(ajaxResponse);
    console.log(data);
    let groups;

    $('#group_tbody').html('');

    for(let i = 0; i < data.length; i++) {
        groups += '<option id="' + data[i].id + '">' + data[i].name + '</option>';

        $('#group_tbody').append(
            '<tr>' +
            '<td><input id=group' + data[i].id + ' class="form-control" type="text" value="' + data[i].name + '"/></td>' +
            '<td><button id=gmod' + data[i].id + ' class="btn btn-success">Edit</button></td>' +
            '<td><button id=gdel' + data[i].id + ' class="btn btn-danger">Delete</button></td>' +
            '</tr>'
        );

        $('#gmod' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('PUT', addressIP + "/group", function () {
            }, 'group_id=' + id + "&name=" + $('#group' + id).val()
            );
        });

        $('#gdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/group", function () {
                ajaxRequest('GET', addressIP + "/groups", showGroups);
            }, 'group_id=' + id
            );
        });
    }

    $('#student_group').html(groups);
    $('#test_group').html(groups);
    $('#group_selector').html(groups);
}


function showStudents(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let students;

    $('#student_tbody').html('');

    let option = document.querySelector("#group_selector > option:checked");

    for(let i = 0; i < data.length; i++) {
        students += '<option id="' + data[i].id + '">' + data[i].first_name + ' ' + data[i].last_name + '</option>';

        $('#student_tbody').append(
            '<tr>' +
            '<td><input id=group_id' + data[i].id + ' class="form-control" type="number" value="' + data[i].group_id + '"/></td>' +
            '<td><input id=first_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].first_name + '"/></td>' +
            '<td><input id=last_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].last_name + '"/></td>' +
            '<td><button id=smod' + data[i].id + ' class="btn btn-success">Edit</button></td>' +
            '<td><button id=sdel' + data[i].id + ' class="btn btn-danger">Delete</button></td>' +
            '</tr>'
        );

        $('#smod' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);
            console.log("click");

            ajaxRequest('PUT', addressIP + "/student", function () {
            }, 'student_id=' + id + "&group_id=" + $('#group_id' + id).val() + "&first_name=" + $('#first_name' + id).val() + "&last_name=" + $('#last_name' + id).val()
            );
        });

        $('#sdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/student", function () {
                ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
                    showStudents(ajaxResponse);
                }, 'group_id=' + option.id );
            }, 'student_id=' + id );
        });
    }

    $('#student_selector').html(students);
}

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
