document.getElementById("student-group").addEventListener("click", function(){
    let option = document.querySelector("#student-group > option:checked");

    console.log(option.id);
});

document.getElementById("group-send").addEventListener("click",function (event) {
    console.log("Add group");

    ajaxRequest('POST', addressIP + "/group", function () {
        console.log("Group added");
    }, 'group_name=' + $('#groupName').val()
    );
});


document.getElementById("student-send").addEventListener("click",function (event) {
    console.log("Add student");

    let option = document.querySelector("#student-group > option:checked");
    console.log(option.id);

    ajaxRequest('POST', addressIP + "/student", function () {
        console.log("student added");
    }, 'group_id=' + option.id + "&first_name=" + $('#studentFirstName').val() + "&last_name=" + $('#studentLastName').val()
    );
});


document.getElementById("admin-send").addEventListener("click",function (event) {
    console.log("Add admin");

    ajaxRequest('POST', addressIP + "/user", function () {
        console.log("admin added");
    }, 'login=' + $('#adminName').val() + "&password=" + $('#adminPassword').val()
    );
});

function showGroups(ajaxResponse) {
    console.log(ajaxResponse);
    let data = JSON.parse(ajaxResponse);
    console.log(data);
    let groups;

    for(let i = 0; i < data.length; i++) {
        groups += '<option id="' + data[i].id + '">' + data[i].name + '</option>';
    }

    $('#student-group').html(groups);
}

function showStudents(ajaxResponse) {
    document.getElementById('grade-panel').style.display = 'none';
    document.getElementById('student-panel').style.display = 'block';

    let data = JSON.parse(ajaxResponse);
    let students;

    for(let i = 0; i < data.length; i++) {
        students += '<option id="' + data[i].id + '">' + data[i].firstname + ' ' + data[i].lastname + '</option>';
    }

    $('#student').html(students);
}

ajaxRequest('GET', addressIP + "/groups", showGroups);