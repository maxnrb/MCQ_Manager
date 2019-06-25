ajaxRequest('GET', addressIP + "/groups", showGroups);
/*ajaxRequest('POST', addressIP + "/question", function (ajaxResponse) {

}, 'test_id=1');*/

/*ajaxRequest('POST', addressIP + "/answer", function (ajaxResponse) {

}, 'question_id=5&test_id=1&good=1');*/


// Select group in home
document.getElementById("home_group").addEventListener("change", function(){
    let option = document.querySelector("#home_group > option:checked");

    $('#home_student').html('<option selected disabled>Choose group first ></option>');
    $('#home_test').html('<option selected disabled>Choose group first ></option>');

    $("#testPicture_div").html('');

    if(option.id !== "") {
        ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
            showStudents(ajaxResponse);
        }, 'group_id=' + option.id );
    }
});


// Select student in home
document.getElementById("home_student").addEventListener("change", function(){
    let option = document.querySelector("#home_student > option:checked");

    $('#home_test').html('<option selected disabled>Choose student first ></option>');

    $("#testPicture_div").html('');

    if(option.id !== "") {
        ajaxRequest('GET', addressIP + "/participations", function (ajaxResponse) {
            showStudentTests(ajaxResponse);
        }, 'student_id=' + option.id );
    }
});

// Select test in home
document.getElementById("home_test").addEventListener("change", function(){
    let option1 = document.querySelector("#home_student > option:checked");
    let option2 = document.querySelector("#home_test > option:checked");

    $("#testPicture_div").html('');

    if(option1.id !== "" && option2.id !== "") {
        ajaxRequest('GET', addressIP + "/test_image", function (ajaxResponse) {

            $("#testPicture_div").append('<img src="'+ ajaxResponse +'" class="img-fluid" alt="Responsive image">')
        }, 'student_id=' + option1.id + "&test_id=" + option2.id );
    }
});

// Selected group in manage student
document.getElementById("group_manageStudentSelector").addEventListener("change", function(){
    let option = document.querySelector("#group_manageStudentSelector > option:checked");

    $('#student_tbody').html('');

    if(option.id !== "") {
        ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
            showStudents(ajaxResponse);
        }, 'group_id=' + option.id );
    }
});


// Selected group in manage test
document.getElementById("group_manageTestSelector").addEventListener("change", function(){
    let option = document.querySelector("#group_manageTestSelector > option:checked");

    $('#test_tbody').html('');

    if(option.id !== "") {
        ajaxRequest('GET', addressIP + "/tests", function (ajaxResponse) {
            showTests(ajaxResponse);
        }, 'group_id=' + option.id );
    }
});

// Create group form
document.getElementById("group_send").addEventListener("click",function (event) {
    ajaxRequest('POST', addressIP + "/group", function () {
        $('#groupName').val('');

        iziToast.show({
            title: 'Group has been created !',
            color: 'green',
            position : "topRight"
        });

    }, 'group_name=' + $('#groupName').val()
    );
});


// Create student form
document.getElementById("student_send").addEventListener("click",function (event) {
    let option = document.querySelector("#student_group > option:checked");

    ajaxRequest('POST', addressIP + "/student", function () {
        $('#studentFirstName').val('');
        $('#studentLastName').val('');

        iziToast.show({
            title: 'Student has been created !',
            color: 'green',
            position : "topRight"
        });

    }, 'group_id=' + option.id + "&first_name=" + $('#studentFirstName').val() + "&last_name=" + $('#studentLastName').val()
    );
});

// Create test form
document.getElementById("test_send").addEventListener("click",function (event) {
    let option = document.querySelector("#test_group > option:checked");

    ajaxRequest('POST', addressIP + "/test", function () {
        $('#createTest_date').val('');
        $('#testName').val('');

        iziToast.show({
            title: 'Test has been created !',
            color: 'green',
            position : "topRight"
        });

        }, 'group_id=' + option.id + "&date=" + $('#createTest_date').val() + "&name=" + $('#testName').val()
    );
});


// Create admin form
document.getElementById("admin_send").addEventListener("click",function (event) {
    ajaxRequest('POST', addressIP + "/user", function () {
        $('#adminName').val('');
        $('#adminPassword').val('');

        iziToast.show({
            title: 'Admin has been created !',
            color: 'green',
            position : "topRight"
        });

        ajaxRequest('GET', addressIP + "/users", showAdmin)
    }, 'login=' + $('#adminName').val() + "&password=" + $('#adminPassword').val()
    );
});


// Load admin list
document.getElementById("adminNav").addEventListener("click", function (event) {
    ajaxRequest('GET', addressIP + "/users", showAdmin)
});


// Correct all students
document.getElementById("correct_allBtn").addEventListener("click", function (event) {
    //ajaxRequest('GET', addressIP + "/users", showAdmin)
});


// Grade all students
document.getElementById("grade_allBtn").addEventListener("click", function (event) {
    //ajaxRequest('GET', addressIP + "/users", showAdmin)
});

function showTestStudents(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let students;

    $('#testStudent_tbody').html('');

    //let option = document.querySelector("#group_manageStudentSelector > option:checked");

    for(let i = 0; i < data.length; i++) {
        students += '<option id="' + data[i].id + '">' + data[i].first_name + ' ' + data[i].last_name + '</option>';

        $('#testStudent_tbody').append(
            '<tr>' +
            '<td><input id=first_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].first_name + '"/></td>' +
            '<td><input id=last_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].last_name + '"/></td>' +
            '<td><input id=last_name' + data[i].id + ' class="form-control" type="text" value="' + data[i].last_name + '"/></td>' +
            '<td><button id=corr' + data[i].id + ' class="btn btn-warning">Correct</button></td>' +
            '<td><button id=grad' + data[i].id + ' class="btn btn-primary">Grade</button></td>' +
            '</tr>'
        );

        $('#corr' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('PUT', addressIP + "/student", function () {
                iziToast.show({
                    title: 'Student has been corrected !', color: 'green', position : "topRight"
                });

            }, 'student_id=' + id + "&group_id=" + $('#group_id' + id).val() + "&first_name=" + $('#first_name' + id).val() + "&last_name=" + $('#last_name' + id).val()
            );
        });

        $('#grad' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/student", function () {
                ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
                    iziToast.show({
                        title: 'Student has been graded !', color: 'green', position : "topRight"
                    });

                    showTestStudents(ajaxResponse);
                }, 'group_id=' + option.id );
            }, 'student_id=' + id );
        });
    }
}

function showGroups(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let groups = '<option selected disabled>Choose group ></option>';

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
                iziToast.show({
                    title: 'Group has been modified !', color: 'green', position : "topRight"
                });

            }, 'group_id=' + id + "&name=" + $('#group' + id).val()
            );
        });

        $('#gdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/group", function () {
                iziToast.show({
                    title: 'Group has been deleted !', color: 'green', position : "topRight"
                });

                ajaxRequest('GET', addressIP + "/groups", showGroups);
            }, 'group_id=' + id
            );
        });
    }

    $('#student_group').html(groups);
    $('#test_group').html(groups);
    $('#group_manageStudentSelector').html(groups);
    $('#group_manageTestSelector').html(groups);
    $('#home_group').html(groups);
}


function showStudents(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let students;

    $('#student_tbody').html('');
    $('#home_test').html('<option selected disabled>Choose student first ></option>');

    let option = document.querySelector("#group_manageStudentSelector > option:checked");
    students += '<option selected disabled>Choose student ></option>';

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

            ajaxRequest('PUT', addressIP + "/student", function () {
                iziToast.show({
                    title: 'Student has been modified !', color: 'green', position : "topRight"
                });

            }, 'student_id=' + id + "&group_id=" + $('#group_id' + id).val() + "&first_name=" + $('#first_name' + id).val() + "&last_name=" + $('#last_name' + id).val()
            );
        });

        $('#sdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/student", function () {
                ajaxRequest('GET', addressIP + "/students", function (ajaxResponse) {
                    iziToast.show({
                        title: 'Student has been deleted !', color: 'green', position : "topRight"
                    });

                    showStudents(ajaxResponse);
                }, 'group_id=' + option.id );
            }, 'student_id=' + id );
        });
    }

    $('#home_student').html(students);
    $('#student_selector').html(students);
}


function showTests(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let option = document.querySelector("#group_manageTestSelector > option:checked");

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

            ajaxRequest('PUT', addressIP + "/test", function () {
                iziToast.show({
                    title: 'Test has been modified !', color: 'green', position : "topRight"
                });

            }, 'test_id=' + id + "&date=" + $('#test_date' + id).val() + "&test_name=" + $('#test_name' + id).val()
            );
        });


        $('#tdel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/test", function () {
                ajaxRequest('GET', addressIP + "/tests", function (ajaxResponse) {
                    iziToast.show({
                        title: 'Test has been deleted !', color: 'green', position : "topRight"
                    });

                    showTests(ajaxResponse);
                }, 'group_id=' + option.id );
            }, 'test_id=' + id );
        });
    }

    $('.datepicker').datepicker({
        uiLibrary: 'bootstrap4'
    });
}

function showAdmin(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);

    $('#admin_tbody').html('');

    for(let i = 0; i < data.length; i++) {
        $('#admin_tbody').append(
            '<tr>' +
            '<td><input id=login' + data[i].id + ' class="form-control" type="text" value="' + data[i].login + '"/></td>' +
            '<td><button id=amod' + data[i].id + ' class="btn btn-success">Edit</button></td>' +
            '<td><button id=adel' + data[i].id + ' class="btn btn-danger">Delete</button></td>' +
            '</tr>'
        );

        $('#amod' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('PUT', addressIP + "/user", function () {
                iziToast.show({
                    title: 'Admin has been modified !', color: 'green', position : "topRight"
                });

            }, 'user_id=' + id + "&login=" + $('#login' + id).val()            );
        });

        $('#adel' + data[i].id).unbind('click').click(function (event) {
            let id = event.target.id.substr(4);

            ajaxRequest('DELETE', addressIP + "/user", function () {
                iziToast.show({
                    title: 'Admin has been deleted !', color: 'green', position : "topRight"
                });

                ajaxRequest('GET', addressIP + "/users", showAdmin);
            }, 'user_id=' + id );
        });
    }
}

function showStudentTests(ajaxResponse) {
    let data = JSON.parse(ajaxResponse);
    let tests;

    let option = document.querySelector("#group_manageStudentSelector > option:checked");
    tests += '<option selected disabled>Choose test ></option>';

    for (let i = 0; i < data.length; i++) {
        tests += '<option id="' + data[i].id + '">' + data[i].name + ' • ' + data[i].date + '</option>';
    }

    $('#home_test').html(tests);
}